/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#ifndef _TRANSFORMATIONS_H_
#define _TRANSFORMATIONS_H_
#include <iostream>
#include <string>
#include "bitstring.h"
#include "types.h"

/**
  * Abstract class that represents a transformation from n bits to n bits.
  */
class Transformation {
public:
    Transformation() {};
    /**
      * Virtual destructor - necessary because this is an abstract class.
      */
    virtual ~Transformation() {};
    /**
      * Abstract method that returns the number of bits of its domain and range.
      */
    virtual unsigned int getWidth() const = 0;
    /**
      * Abstract method that applies the transformation onto the parameter
      * @a state.
      *
      * @param  state   A buffer on which to apply the transformation.
      *                 The state must have a size of at least
      *                 ceil(getWidth()/8.0) bytes.
      */
    virtual void operator()(UINT8 * state) const = 0;
    /**
      * Abstract method that returns a string with a description of itself.
      */
    virtual std::string getDescription() const = 0;
    /**
      * Method that prints a brief description of the transformation.
      */
    friend std::ostream& operator<<(std::ostream& a, const Transformation& transformation);
};

/**
  * Abstract class that represents a permutation from n bits to n bits.
  */
class Permutation : public Transformation {
public:
    Permutation() : Transformation() {};
    /**
      * Abstract method that applies the <em>inverse</em> of the permutation
      * onto the parameter @a state.
      *
      * @param  state   A buffer on which to apply the inverse permutation.
      *                 The state must have a size of at least
      *                 ceil(getWidth()/8.0) bytes.
      */
    virtual void inverse(UINT8 * state) const = 0;
};

/**
  * Class that implements the simplest possible permutation: the identity.
  */
class Identity : public Permutation {
protected:
    unsigned int width;
public:
    Identity(unsigned int aWidth) : Permutation(), width(aWidth) {};
    virtual unsigned int getWidth() const { return width; }
    virtual void operator()(UINT8 * state) const {(void)state;}
    virtual std::string getDescription() const { return "Identity"; }
    virtual void inverse(UINT8 * state) const {(void)state;}
};

/**
 * Class implementing an iterable permutation
 */
class BaseIterableTransformation
{
	public:
		const unsigned int            width;
		const unsigned int            rounds;

		BaseIterableTransformation(unsigned int width, unsigned int rounds) : width(width), rounds(rounds) {}

		virtual BitString operator()(const BitString &state) const = 0;
};

template<class T>
class IterableTransformation : public BaseIterableTransformation
{
	protected:
		T                     f;

	public:
		IterableTransformation(unsigned int width) : BaseIterableTransformation(width, 0), f(width) {}
		IterableTransformation(unsigned int width, unsigned int rounds) : BaseIterableTransformation(width, rounds), f(width, rounds) {}

		BitString operator()(const BitString &state) const
		{
			BitString state2 = state;
			f(state2.array());
			return state2;
		}
};

#endif
