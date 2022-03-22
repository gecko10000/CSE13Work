#pragma once
#include "metric.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern uint32_t noiselimit; // Number of noise words to filter out.

typedef struct Text Text;

Text *text_create(FILE *infile, Text *noise);

void text_delete(Text **text);

double text_dist(Text *text1, Text *text2, Metric metric);

double text_frequency(Text *text, char *word);

bool text_contains(Text *text, char *word);

void text_print(Text *text);
