/**
 * @file log.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef LOG_H
#define LOG_H

#ifdef __ARDUINO__
#include <Arduino.h>
#else
#include <stdio.h>
#endif

#define LOG_INF(_fmt, ...) fprintf(stdout, _fmt "\n", ##__VA_ARGS__);
#define LOG_ERR(_fmt, ...) fprintf(stderr, _fmt "\n", ##__VA_ARGS__);

#endif // LOG_H