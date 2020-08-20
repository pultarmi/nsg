//
// Created by milan on 20/08/2020.
//

#ifndef EFANNA2E_AUX_H
#define EFANNA2E_AUX_H

#endif //EFANNA2E_AUX_H

#pragma once
#ifndef  OVERRIDE_API
#ifdef POSTGRES_MATCHER_EXPORTS
#ifdef WIN32
#define POSTGRES_MATCHER_API __declspec(dllexport)
#else
#define POSTGRES_MATCHER_API __declspec(dllimport)
#endif
#else
#define POSTGRES_MATCHER_API
#endif
#else
#define POSTGRES_MATCHER_API
#endif // ! OVERRIDE_API