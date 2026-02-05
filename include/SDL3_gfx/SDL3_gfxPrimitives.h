/* 

SDL3_gfxPrimitives.h: graphics primitives for SDL

Copyright (C) 2012-2014  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

*/

#ifndef _SDL3_gfxPrimitives_h
#define _SDL3_gfxPrimitives_h

#include <cmath>
#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif

#include <SDL3/SDL.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* ----- Versioning */

#define SDL3_GFXPRIMITIVES_MAJOR	1
#define SDL3_GFXPRIMITIVES_MINOR	0
#define SDL3_GFXPRIMITIVES_MICRO	0


/* ---- Function Prototypes */

#ifdef _MSC_VER
#  if defined(DLL_EXPORT) && !defined(LIBSDL3_GFX_DLL_IMPORT)
#    define SDL3_GFXPRIMITIVES_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL3_GFX_DLL_IMPORT
#      define SDL3_GFXPRIMITIVES_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL3_GFXPRIMITIVES_SCOPE
#  define SDL3_GFXPRIMITIVES_SCOPE extern
#endif

/* Filled Polygon */
#include <stdlib.h>
#define POLYSIZE 16384
SDL3_GFXPRIMITIVES_SCOPE bool filledPolygonColor(SDL_Renderer *renderer, const float *vx, const float *vy, Sint32 n, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool filledPolygonRGBA(SDL_Renderer *renderer, const float *vx,
                                                const float *vy, Sint32 n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);


static int _gfxPrimitivesCompareFloat2(const void *a, const void *b) {
    float diff = *((float *) a + 1) - *((float *) b + 1);
    if (diff != 0.0) return (diff > 0) - (diff < 0);
    diff = *(float *) a - *(float *) b;
    return (diff > 0) - (diff < 0);
}

inline int aaFilledPolygonRGBA(SDL_Renderer *renderer, const double *vx, const double *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int i, j, xi, yi, result;
    double x1, x2, y0, y1, y2, minx, maxx, prec;
    float *list, *strip;

    if (n < 3)
        return -1;

    result = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Find extrema:
    minx = 99999.0;
    maxx = -99999.0;
    prec = 0.00001;
    for (i = 0; i < n; i++) {
        double x = vx[i];
        double y = fabs(vy[i]);
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y > prec) prec = y;
    }
    minx = floor(minx);
    maxx = floor(maxx);
    prec = floor(pow(2, 19) / prec);

    // Allocate main array, this determines the maximum polygon size and complexity:
    list = (float *) malloc(POLYSIZE * sizeof(float));
    if (list == NULL)
        return -2;

    // Build vertex list.  Special x-values used to indicate vertex type:
    // x = -100001.0 indicates /\, x = -100003.0 indicates \/, x = -100002.0 neither
    yi = 0;
    y0 = floor(vy[n - 1] * prec) / prec;
    y1 = floor(vy[0] * prec) / prec;
    for (i = 1; i <= n; i++) {
        if (yi > POLYSIZE - 4) {
            free(list);
            return -2;
        }
        y2 = floor(vy[i % n] * prec) / prec;
        if (((y1 < y2) - (y1 > y2)) == ((y0 < y1) - (y0 > y1))) {
            list[yi++] = -100002.0;
            list[yi++] = y1;
            list[yi++] = -100002.0;
            list[yi++] = y1;
        } else {
            if (y0 != y1) {
                list[yi++] = (y1 < y0) - (y1 > y0) - 100002.0;
                list[yi++] = y1;
            }
            if (y1 != y2) {
                list[yi++] = (y1 < y2) - (y1 > y2) - 100002.0;
                list[yi++] = y1;
            }
        }
        y0 = y1;
        y1 = y2;
    }
    xi = yi;

    // Sort vertex list:
    qsort(list, yi / 2, sizeof(float) * 2, _gfxPrimitivesCompareFloat2);

    // Append line list to vertex list:
    for (i = 1; i <= n; i++) {
        double x, y;
        double d = 0.5 / prec;

        x1 = vx[i - 1];
        y1 = floor(vy[i - 1] * prec) / prec;
        x2 = vx[i % n];
        y2 = floor(vy[i % n] * prec) / prec;

        if (y2 < y1) {
            double tmp;
            tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        if (y2 != y1)
            y0 = (x2 - x1) / (y2 - y1);

        for (j = 1; j < xi; j += 4) {
            y = list[j];
            if (((y + d) <= y1) || (y == list[j + 4]))
                continue ;
            if ((y -= d) >= y2)
                break ;
            if (yi > POLYSIZE - 4) {
                free(list);
                return -2;
            }
            if (y > y1) {
                list[yi++] = x1 + y0 * (y - y1);
                list[yi++] = y;
            }
            y += d * 2.0;
            if (y < y2) {
                list[yi++] = x1 + y0 * (y - y1);
                list[yi++] = y;
            }
        }

        y = floor(y1) + 1.0;
        while (y <= y2) {
            x = x1 + y0 * (y - y1);
            if (yi > POLYSIZE - 2) {
                free(list);
                return -2;
            }
            list[yi++] = x;
            list[yi++] = y;
            y += 1.0;
        }
    }

    // Sort combined list:
    qsort(list, yi / 2, sizeof(float) * 2, _gfxPrimitivesCompareFloat2);

    // Plot lines:
    strip = (float *) malloc((maxx - minx + 2) * sizeof(float));
    if (strip == NULL) {
        free(list);
        return -1;
    }
    memset(strip, 0, (maxx - minx + 2) * sizeof(float));
    n = yi;
    yi = list[1];
    j = 0;

    for (i = 0; i < n - 7; i += 4) {
        float x1 = list[i + 0];
        float y1 = list[i + 1];
        float x3 = list[i + 2];
        float x2 = list[i + j + 0];
        float y2 = list[i + j + 1];
        float x4 = list[i + j + 2];

        if (x1 + x3 == -200002.0)
            j += 4;
        else if (x1 + x3 == -200006.0)
            j -= 4;
        else if ((x1 >= minx) && (x2 >= minx)) {
            if (x1 > x2) {
                float tmp = x1;
                x1 = x2;
                x2 = tmp;
            }
            if (x3 > x4) {
                float tmp = x3;
                x3 = x4;
                x4 = tmp;
            }

            for (xi = x1 - minx; xi <= x4 - minx; xi++) {
                float u, v;
                float x = minx + xi;
                if (x < x2) u = (x - x1 + 1) / (x2 - x1 + 1);
                else u = 1.0;
                if (x >= x3 - 1) v = (x4 - x) / (x4 - x3 + 1);
                else v = 1.0;
                if ((u > 0.0) && (v > 0.0))
                    strip[xi] += (y2 - y1) * (u + v - 1.0);
            }
        }

        if ((yi == (list[i + 5] - 1.0)) || (i == n - 8)) {
            for (xi = 0; xi <= maxx - minx; xi++) {
                if (strip[xi] != 0.0) {
                    if (strip[xi] >= 0.996) {
                        int x0 = xi;
                        while (strip[++xi] >= 0.996);
                        xi--;
                        result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
                        result |= SDL_RenderLine(renderer, minx + x0, yi, minx + xi, yi);
                    } else {
                        result |= SDL_SetRenderDrawColor(renderer, r, g, b, a * strip[xi]);
                        result |= SDL_RenderPoint(renderer, minx + xi, yi);
                    }
                }
            }
            memset(strip, 0, (maxx - minx + 2) * sizeof(float));
            yi++;
        }
    }

    // Free arrays:
    free(list);
    free(strip);
    return result;
}

// returns Returns 0 on success, -1 on failure.
inline int aaFilledPolygonColor(SDL_Renderer *renderer, const double *vx, const double *vy, int n, Uint32 color) {
    Uint8 *c = (Uint8 *) &color;
    return aaFilledPolygonRGBA(renderer, vx, vy, n, c[0], c[1], c[2], c[3]);
}

inline int aaFilledPieOuterRGBA(
    SDL_Renderer *renderer,
    float cx, float cy,
    float radius,
    float thickness,
    float start, float end,
    Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (radius <= 0 || thickness <= 0 || start == end)
        return -1;

    float inner = radius - thickness;
    if (inner <= 0)
        inner = 0.1f;

    start = fmodf(start, 360.0f) * M_PI / 180.0f;
    end = fmodf(end, 360.0f) * M_PI / 180.0f;
    while (start >= end)
        end += 2.0f * M_PI;

    int steps = int((end - start) * radius / 4.0f);
    if (steps < 8)
        steps = 8;
    else if (steps > 180)
        steps = 180;


    int verts = steps * 2;
    float *vx = (float *) malloc(sizeof(float) * verts);
    float *vy = (float *) malloc(sizeof(float) * verts);
    if (!vx || !vy) {
        free(vx);
        free(vy);
        return -1;
    }

    // внешняя дуга
    for (int i = 0; i < steps; ++i) {
        float a0 = start + (end - start) * i / (steps - 1);
        vx[i] = cx + radius * cosf(a0);
        vy[i] = cy + radius * sinf(a0);
    }

    // внутренняя дуга (в обратном порядке)
    for (int i = 0; i < steps; ++i) {
        float a0 = end - (end - start) * i / (steps - 1);
        vx[steps + i] = cx + inner * cosf(a0);
        vy[steps + i] = cy + inner * sinf(a0);
    }

    int res = filledPolygonRGBA(renderer, vx, vy, verts, r, g, b, a);

    free(vx);
    free(vy);
    return res;
}


inline int aaFilledPieOuterColor(
    SDL_Renderer *renderer,
    float cx, float cy,
    float radius,
    float thickness,
    float start, float end,
    Uint32 color) {
    Uint8 *c = (Uint8 *) &color;
    return aaFilledPieOuterRGBA(
        renderer,
        cx, cy,
        radius,
        thickness,
        start, end,
        c[0], c[1], c[2], c[3]
    );
}

/* Note: all ___Color routines expect the color to be in format 0xRRGGBBAA */
/* Note: all ___Color routines expect the color to be in format 0xRRGGBBAA */
inline int hollowCircle(SDL_Renderer *renderer, float cx, float cy, float rx, float borderThickness, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if ((rx <= 0.0) || (ry <= 0.0) || (borderThickness <= 0.0))
        return -1;

    int result = SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    float outerRx = rx;
    float outerRy = ry;
    float innerRx = rx - borderThickness;
    float innerRy = ry - borderThickness;

    if (innerRx < 0) innerRx = 0;
    if (innerRy < 0) innerRy = 0;

    for (int yi = static_cast<int>(cy - outerRy); yi <= static_cast<int>(cy + outerRy); yi++) {
        float y = yi + 0.5f - cy;
        float y2 = y * y;

        // рассчитываем x для внешнего и внутреннего эллипса
        float outerX = outerRx * sqrtf(1.0f - y2 / (outerRy * outerRy));
        float innerX = (innerRx > 0 && innerRy > 0 && y2 < innerRy * innerRy) ? innerRx * sqrtf(1.0f - y2 / (innerRy * innerRy)) : 0;

        if (outerX > 0) {
            result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

            // рисуем только кольцо, между innerX и outerX
            SDL_RenderLine(renderer, static_cast<int>(cx - outerX), yi, static_cast<int>(cx - innerX), yi);
            SDL_RenderLine(renderer, static_cast<int>(cx + innerX), yi, static_cast<int>(cx + outerX), yi);
        }
    }

    return result;
}


/* Pixel */

SDL3_GFXPRIMITIVES_SCOPE bool pixelColor(SDL_Renderer *renderer, float x, float y, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool pixelRGBA(SDL_Renderer *renderer, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Horizontal line */

SDL3_GFXPRIMITIVES_SCOPE bool hlineColor(SDL_Renderer *renderer, float x1, float x2, float y, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool hlineRGBA(SDL_Renderer *renderer, float x1, float x2, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Vertical line */

SDL3_GFXPRIMITIVES_SCOPE bool vlineColor(SDL_Renderer *renderer, float x, float y1, float y2, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool vlineRGBA(SDL_Renderer *renderer, float x, float y1, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Rectangle */

SDL3_GFXPRIMITIVES_SCOPE bool rectangleColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool rectangleRGBA(SDL_Renderer *renderer, float x1, float y1,
                                            float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Rounded-Corner Rectangle */

SDL3_GFXPRIMITIVES_SCOPE bool roundedRectangleColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float rad, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool roundedRectangleRGBA(SDL_Renderer *renderer, float x1, float y1,
                                                   float x2, float y2, float rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled rectangle (Box) */

SDL3_GFXPRIMITIVES_SCOPE bool boxColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool boxRGBA(SDL_Renderer *renderer, float x1, float y1, float x2,
                                      float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Rounded-Corner Filled rectangle (Box) */

SDL3_GFXPRIMITIVES_SCOPE bool roundedBoxColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float rad, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool roundedBoxRGBA(SDL_Renderer *renderer, float x1, float y1, float x2,
                                             float y2, float rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Line */

SDL3_GFXPRIMITIVES_SCOPE bool lineColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool lineRGBA(SDL_Renderer *renderer, float x1, float y1,
                                       float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* AA Line */

SDL3_GFXPRIMITIVES_SCOPE bool aalineColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool aalineRGBA(SDL_Renderer *renderer, float x1, float y1,
                                         float x2, float y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Thick Line */
SDL3_GFXPRIMITIVES_SCOPE bool thickLineColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2,
                                             float width, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool thickLineRGBA(SDL_Renderer *renderer, float x1, float y1, float x2, float y2,
                                            float width, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Circle */

SDL3_GFXPRIMITIVES_SCOPE bool circleColor(SDL_Renderer *renderer, float x, float y, float rad, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool circleRGBA(SDL_Renderer *renderer, float x, float y, float rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Arc */

SDL3_GFXPRIMITIVES_SCOPE bool arcColor(SDL_Renderer *renderer, float x, float y, float rad, Sint32 start, Sint32 end, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool arcRGBA(SDL_Renderer *renderer, float x, float y, float rad, Sint32 start, Sint32 end,
                                      Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* AA Circle */

SDL3_GFXPRIMITIVES_SCOPE bool aacircleColor(SDL_Renderer *renderer, float x, float y, float rad, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool aacircleRGBA(SDL_Renderer *renderer, float x, float y,
                                           float rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled Circle */

SDL3_GFXPRIMITIVES_SCOPE bool filledCircleColor(SDL_Renderer *renderer, float x, float y, float r, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool filledCircleRGBA(SDL_Renderer *renderer, float x, float y,
                                               float rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Ellipse */

SDL3_GFXPRIMITIVES_SCOPE bool ellipseColor(SDL_Renderer *renderer, float x, float y, float rx, float ry, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool ellipseRGBA(SDL_Renderer *renderer, float x, float y,
                                          float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* AA Ellipse */

SDL3_GFXPRIMITIVES_SCOPE bool aaellipseColor(SDL_Renderer *renderer, float x, float y, float rx, float ry, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool aaellipseRGBA(SDL_Renderer *renderer, float x, float y,
                                            float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled Ellipse */

SDL3_GFXPRIMITIVES_SCOPE bool filledEllipseColor(SDL_Renderer *renderer, float x, float y, float rx, float ry, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool filledEllipseRGBA(SDL_Renderer *renderer, float x, float y,
                                                float rx, float ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Pie */

SDL3_GFXPRIMITIVES_SCOPE bool pieColor(SDL_Renderer *renderer, float x, float y, float rad,
                                       Sint32 start, Sint32 end, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool pieRGBA(SDL_Renderer *renderer, float x, float y, float rad,
                                      Sint32 start, Sint32 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled Pie */

SDL3_GFXPRIMITIVES_SCOPE bool filledPieColor(SDL_Renderer *renderer, float x, float y, float rad,
                                             Sint32 start, Sint32 end, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool filledPieRGBA(SDL_Renderer *renderer, float x, float y, float rad,
                                            Sint32 start, Sint32 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Trigon */

SDL3_GFXPRIMITIVES_SCOPE bool trigonColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float x3, float y3, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool trigonRGBA(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float x3, float y3,
                                         Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* AA-Trigon */

SDL3_GFXPRIMITIVES_SCOPE bool aatrigonColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float x3, float y3, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool aatrigonRGBA(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float x3, float y3,
                                           Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled Trigon */

SDL3_GFXPRIMITIVES_SCOPE bool filledTrigonColor(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float x3, float y3, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool filledTrigonRGBA(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float x3, float y3,
                                               Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Polygon */

SDL3_GFXPRIMITIVES_SCOPE bool polygonColor(SDL_Renderer *renderer, const float *vx, const float *vy, Sint32 n, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool polygonRGBA(SDL_Renderer *renderer, const float *vx, const float *vy,
                                          Sint32 n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* AA-Polygon */

SDL3_GFXPRIMITIVES_SCOPE bool aapolygonColor(SDL_Renderer *renderer, const float *vx, const float *vy, Sint32 n, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool aapolygonRGBA(SDL_Renderer *renderer, const float *vx, const float *vy,
                                            Sint32 n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);


/* Textured Polygon */

SDL3_GFXPRIMITIVES_SCOPE bool texturedPolygon(SDL_Renderer *renderer, const float *vx, const float *vy, Sint32 n, SDL_Surface *texture, Sint32 texture_dx, Sint32 texture_dy);

/* Bezier */

SDL3_GFXPRIMITIVES_SCOPE bool bezierColor(SDL_Renderer *renderer, const float *vx, const float *vy, Sint32 n, Sint32 s, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool bezierRGBA(SDL_Renderer *renderer, const float *vx, const float *vy,
                                         Sint32 n, Sint32 s, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Characters/Strings */

SDL3_GFXPRIMITIVES_SCOPE void gfxPrimitivesSetFont(const void *fontdata, Uint32 cw, Uint32 ch);

SDL3_GFXPRIMITIVES_SCOPE void gfxPrimitivesSetFontRotation(Uint32 rotation);

SDL3_GFXPRIMITIVES_SCOPE bool characterColor(SDL_Renderer *renderer, float x, float y, char c, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool characterRGBA(SDL_Renderer *renderer, float x, float y, char c, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

SDL3_GFXPRIMITIVES_SCOPE bool stringColor(SDL_Renderer *renderer, float x, float y, const char *s, Uint32 color);

SDL3_GFXPRIMITIVES_SCOPE bool stringRGBA(SDL_Renderer *renderer, float x, float y, const char *s, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif				/* _SDL3_gfxPrimitives_h */
