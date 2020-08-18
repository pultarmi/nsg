//
// Created by milan on 13/08/2020.
//

#ifndef EFANNA2E_RUN_H
#define EFANNA2E_RUN_H



class run {
};

//#endif //KNNC_MAIN_H
#pragma once

#ifdef WIN32
#ifdef POSTGRES_MATCHER_EXPORTS
#define POSTGRES_MATCHER_API __declspec(dllexport)
#else
#define POSTGRES_MATCHER_API __declspec(dllimport)
#endif
#else
#define POSTGRES_MATCHER_API
#endif

#include <iostream>
#include <stdint.h>
#include <unistd.h>

#define QS_DISTANCE_FUNCTION_EUCLIDEAN 0
#define QS_DISTANCE_FUNCTION_EUCLIDEAN_AVX 1

extern "C" {
struct POSTGRES_MATCHER_API QsMatcherResult {
    int64_t id;
    int64_t subjectId;
    float distance;
};

POSTGRES_MATCHER_API void* CreateMatcher(const char* connectionString, int32_t fetchSize, int32_t maxTemplatesCount, int32_t* providerIds, int32_t* embeddingLengths, float* ratios, int32_t providerCount);
POSTGRES_MATCHER_API void DeleteMatcher(void* instance);
POSTGRES_MATCHER_API bool Matcher_reload(void* instance);
POSTGRES_MATCHER_API int32_t Matcher_matchTopN(void* instance, int32_t n, int32_t partitionId, const int32_t* imagesProviderIds, int32_t imagesProviderCount, const float* imagesEmbeddings, int32_t imagesCount, int32_t distanceFunction);
POSTGRES_MATCHER_API int32_t Matcher_getOrderOfSubject(void* instance, int64_t subjectId, int32_t partitionId, const int32_t* imagesProviderIds, int32_t imagesProviderCount, const float* imagesEmbeddings, int32_t imagesCount, int32_t distanceFunction);
POSTGRES_MATCHER_API struct QsMatcherResult Matcher_popResult(void* instance);
POSTGRES_MATCHER_API bool Matcher_copyResults(void* instance, struct QsMatcherResult* results);
POSTGRES_MATCHER_API int32_t Matcher_getResultsSize(void* instance);
POSTGRES_MATCHER_API int32_t Matcher_getTemplatesCount(void* instance);
POSTGRES_MATCHER_API const char* Matcher_getVersion(void* instance);
POSTGRES_MATCHER_API void Matcher_clear(void* instance);
}


#endif