/* render_weather.h - Parallax scrolling and weather: rain, snow, lightning, fog, searchlights and traffic. */
#ifndef NCS_RENDER_WEATHER_H
#define NCS_RENDER_WEATHER_H

void animateLayers(void);
void composite(int dimK);
/* storm lightning: at most one strike every 5.5 s window; returns seconds since this window's strike (<0 = none yet) */
float strikeAge(int *win, int *bx);
void snow(void);
void ambienceBack(void);
void ambienceFront(void);

#endif
