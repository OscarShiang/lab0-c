/** dude, is my code constant time?
 *
 * This file measures the execution time of a given function many times with
 * different inputs and performs a Welch's t-test to determine if the function
 * runs in constant time or not. This is essentially leakage detection, and
 * not a timing attack.
 *
 * Notes:
 *
 *  - the execution time distribution tends to be skewed towards large
 *    timings, leading to a fat right tail. Most executions take little time,
 *    some of them take a lot. We try to speed up the test process by
 *    throwing away those measurements with large cycle count. (For example,
 *    those measurements could correspond to the execution being interrupted
 *    by the OS.) Setting a threshold value for this is not obvious; we just
 *    keep the x% percent fastest timings, and repeat for several values of x.
 *
 *  - the previous observation is highly heuristic. We also keep the uncropped
 *    measurement time and do a t-test on that.
 *
 *  - we also test for unequal variances (second order test), but this is
 *    probably redundant since we're doing as well a t-test on cropped
 *    measurements (non-linear transform)
 *
 *  - as long as any of the different test fails, the code will be deemed
 *    variable time.
 *
 */

#include "fixture.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../console.h"
#include "../random.h"
#include "constant.h"
#include "percentile.h"
#include "ttest.h"

#define enough_measurements 10000
#define test_tries 10
#define number_percentiles 100
#define number_tests 102
/* 1 (first order uncropped)
 * 100 (first order cropped)
 * 1 (second order cropped)
 */

extern const int drop_size;
extern const size_t chunk_size;
extern const size_t number_measurements;
static t_ctx *t[number_tests];

static int64_t percentiles[number_percentiles] = {0};

/* threshold values for Welch's t-test */
#define t_threshold_bananas                                                  \
    500                         /* Test failed with overwhelming probability \
                                 */
#define t_threshold_moderate 10 /* Test failed */

static void __attribute__((noreturn)) die(void)
{
    exit(111);
}

static void differentiate(int64_t *exec_times,
                          int64_t *before_ticks,
                          int64_t *after_ticks)
{
    for (size_t i = 0; i < number_measurements; i++) {
        exec_times[i] = after_ticks[i] - before_ticks[i];
    }
}

static void update_statistics(int64_t *exec_times, uint8_t *classes)
{
    for (size_t i = 0; i < number_measurements; i++) {
        int64_t difference = exec_times[i];
        /* Cpu cycle counter overflowed or dropped measurement */
        if (difference <= 0) {
            continue;
        }

        /* do  first order t-test on the execution time */
        t_push(t[0], difference, classes[i]);


        /* compute first order t-test with cropped data */
        for (int crop_idx = 0; crop_idx < number_percentiles; ++crop_idx) {
            if (difference < percentiles[crop_idx]) {
                t_push(t[crop_idx + 1], difference, classes[i]);
            }
        }
        /* second order t-test with uncropped data */
        if (t[0]->n[0] > enough_measurements) {
            double centered = (double) difference - t[0]->mean[classes[i]];
            t_push(t[1 + number_percentiles], centered * centered, classes[i]);
        }
    }
}

static void prepare_percentiles(int64_t *exec_times)
{
    for (size_t i = 0; i < number_percentiles; ++i) {
        percentiles[i] = percentile(
            exec_times,
            1 - (pow(0.5, 10 * (double) (i + 1) / number_percentiles)),
            number_measurements);
    }
}

/* search for the index of the max t value */
static int max_test(void)
{
    int ret = 0;
    double max = 0;
    for (size_t i = 0; i < number_tests - 1; i++) {
        if (t[i]->n[0] > enough_measurements) {
            double x = fabs(t_compute(t[i]));
            if (max < x) {
                max = x;
                ret = i;
            }
        }
    }
#if 0
    double x = fabs(t_compute(t[0]));
    if (max < x) {
        max = x;
        ret = 0;
    }
    x = fabs(t_compute(t[101]));
    if (max < x) {
        max = x;
        ret = 101;
    }
#endif
    return ret;
}

static bool report(void)
{
    int max_idx = max_test();
    double max_t = fabs(t_compute(t[max_idx]));
    double number_traces_max_t = t[max_idx]->n[0] + t[max_idx]->n[1];
    double max_tau = max_t / sqrt(number_traces_max_t);

#ifndef DUT_TEST
    printf("\033[A\033[2K");
#else
    printf((max_idx != 0) ? "[max_test] " : "");
#endif

    printf("meas: %7.2lf M, ", (number_traces_max_t / 1e6));
    if (number_traces_max_t < enough_measurements) {
        printf("not enough measurements (%.0f still to go).\n",
               enough_measurements - number_traces_max_t);
        return false;
    }

    /*
     * max_t: the t statistic value
     * max_tau: a t value normalized by sqrt(number of measurements).
     *          this way we can compare max_tau taken with different
     *          number of measurements. This is sort of "distance
     *          between distributions", independent of number of
     *          measurements.
     * (5/tau)^2: how many measurements we would need to barely
     *            detect the leak, if present. "barely detect the
     *            leak" = have a t value greater than 5.
     */
#ifndef DUT_TEST
    printf("max t: %+7.2f, max tau: %.2e, (5/tau)^2: %.2e.\n", max_t, max_tau,
           (double) (5 * 5) / (double) (max_tau * max_tau));
#else
    printf("max t: %+7.2f, max tau: %.2e, (5/tau)^2: %.2e.", max_t, max_tau,
           (double) (5 * 5) / (double) (max_tau * max_tau));
#endif
    if (max_t > t_threshold_bananas) {
#ifdef DUT_TEST
        printf(" Definitely not constant time.\n");
#endif
        return false;
    } else if (max_t > t_threshold_moderate) {
#ifdef DUT_TEST
        printf(" Probably not constant time.\n");
#endif
        return false;
    } else { /* max_t < t_threshold_moderate */
#ifdef DUT_TEST
        printf(" For the moment, maybe constant time.\n");
#endif
        return true;
    }
}

static bool doit(int mode)
{
    int64_t *before_ticks = calloc(number_measurements + 1, sizeof(int64_t));
    int64_t *after_ticks = calloc(number_measurements + 1, sizeof(int64_t));
    int64_t *exec_times = calloc(number_measurements, sizeof(int64_t));
    uint8_t *classes = calloc(number_measurements, sizeof(uint8_t));
    uint8_t *input_data =
        calloc(number_measurements * chunk_size, sizeof(uint8_t));

    if (!before_ticks || !after_ticks || !exec_times || !classes ||
        !input_data) {
        die();
    }

    prepare_inputs(input_data, classes);
    measure(before_ticks, after_ticks, input_data, mode);
    differentiate(exec_times, before_ticks, after_ticks);

    // only compute percentiles when the array is empty
    if (!percentiles[number_percentiles - 1])
        prepare_percentiles(exec_times);

    update_statistics(exec_times, classes);

    bool ret = report();

    free(before_ticks);
    free(after_ticks);
    free(exec_times);
    free(classes);
    free(input_data);

    return ret;
}

static void init_once(void)
{
    init_dut();
}

bool is_insert_tail_const(void)
{
    bool result = false;
    // t = malloc(sizeof(t_ctx));
    for (int i = 0; i < number_tests; ++i) {
        t[i] = malloc(sizeof(t_ctx));
        t_init(t[i]);
    }

    for (int cnt = 0; cnt < test_tries; ++cnt) {
        printf("Testing insert_tail...(%d/%d)\n\n", cnt, test_tries);
        init_once();
        for (int i = 0;
             i <
             enough_measurements / (number_measurements - drop_size * 2) + 1;
             ++i)
            result = doit(0);
        printf("\033[A\033[2K\033[A\033[2K");
        if (result == true)
            break;
    }
    for (int i = 0; i < number_measurements; ++i)
        free(t[i]);
    return result;
}

bool is_size_const(void)
{
    bool result = false;
    // t = malloc(sizeof(t_ctx));
    for (int cnt = 0; cnt < test_tries; ++cnt) {
        printf("Testing size...(%d/%d)\n\n", cnt, test_tries);
        init_once();
        for (int i = 0;
             i <
             enough_measurements / (number_measurements - drop_size * 2) + 1;
             ++i)
            result = doit(1);
        printf("\033[A\033[2K\033[A\033[2K");
        if (result == true)
            break;
    }
    for (int i = 0; i < number_measurements; ++i)
        free(t[i]);
    return result;
}

void test_const(int mode)
{
    assert(mode == 1 || mode == 0);

    for (int i = 0; i < number_tests; ++i) {
        t[i] = malloc(sizeof(t_ctx));
        assert(t[i]);
        t_init(t[i]);
    }
    init_once();

    while (true)
        doit(mode);

    for (int i = 0; i < number_measurements; ++i)
        free(t[i]);
}
