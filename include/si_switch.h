/**
 * @file si_switch.h
 * @author Brandon McClenathan (brandon@mcclenathan.us)
 * @brief
 * @date 2024-03-01
 *
 * North Carolina State University Class of 2024
 * Mechanical Engineering Senior Design, Water Tunnel, Group 5
 *
 */

#ifndef SI_SWITCH_H
#define SI_SWITCH_H

typedef enum {
  SW_LIMIT_MIN,
  SW_LIMIT_MAX,
  SW_RUN,
  SW_ENCODER,

  SW_CNT,
} si_switch_t;

bool si_switch_get(si_switch_t sw);

#endif // SI_SWITCH_H