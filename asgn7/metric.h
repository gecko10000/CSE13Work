#pragma once

typedef enum { EUCLIDEAN, MANHATTAN, COSINE } Metric;

static const char *metric_names[] = { [EUCLIDEAN] = "Euclidean distance",
    [MANHATTAN] = "Manhattan distance",
    [COSINE] = "Cosine distance" };
