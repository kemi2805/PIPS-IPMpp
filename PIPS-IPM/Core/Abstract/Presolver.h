/*
 * Presolver.h
 *
 *  Created on: 26.01.2018
 *      Author: bzfrehfe
 */

#ifndef PIPS_IPM_CORE_ABSTRACT_PRESOLVER_H_
#define PIPS_IPM_CORE_ABSTRACT_PRESOLVER_H_

#include "pipsport.h"
#include <cstddef>

class Data;
class Postsolver;
/**  * @defgroup Preprocessing
 *
 * Interior-point presolvers
 * @{
 */

/**
 * Abstract base class for presolvers.
 */


class Presolver
{
public:

   Presolver(const Data& prob, Postsolver* postsolver = nullptr) : origprob{prob}, postsolver{postsolver}{};
   virtual ~Presolver() = default;

   /** presolve and return pointer to presolved data */
   virtual Data* presolve() = 0;

protected:
   const Data& origprob;
   Postsolver* const postsolver{};

};

//@}


#endif /* PIPS_IPM_CORE_ABSTRACT_PRESOLVER_H_ */
