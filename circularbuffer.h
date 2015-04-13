/* *****************************************************************************
*  A templated circular buffer.
*  Copyright 2015 Keith Vasilakes
*  ***************************************************************************** */

#ifndef __CIRCULAR_BUFFER_H
#define __CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stddef.h>

template <class T, uint16_t MAX_ENTRIES> class CircularBuffer
{
  T Buffer[MAX_ENTRIES];

  // Head is the location of the last value added to the
  // circular buffer. 
  int In;
  int Out;

  // Track the number of entries stored in the buffer. 
  // Never greater than MAX_ENTRIES;
  int Size;

public:
  CircularBuffer():
  In(0),
  Out(0),
  Size(0)
    /* Initializes an empty buffer.  */
  {
  }

  uint8_t ElementSize()
  {
    return sizeof(T);
  }

  bool IsEmpty()
    /* Returns true if no data has been stored in the buffer, 
    and false if at least one value has been stored. */
  {
    return Size == 0;
  }

    /* Add a new value to the end of the buffer. If the
    buffer has exceed its intrinsic capacity, this will
    overwrite old entries. */
  bool Put(const T c)
  {
	uint16_t in = In+1;
	
    if (in+1 >= MAX_ENTRIES -1)
	{
      in = 0;
	}
	
	if(in != Out)
	{
		Buffer[In] = c;
		
		In = in;
		return true;
	}
	
	return false;
  }
  
  //allows adding after buffer is full, 
  //Will overwrite old data
  void Add(T c)
  {
	  Buffer[In++] = c;

	  //handle wrap
	  if( In >= MAX_ENTRIES-1)
	  In = 0;
  }

//Get a byte from the buffer
//Removes byte
bool Get(T &c)
{
	//buffer is empty?
	if(In != Out)
	{
		//not empty yet
		c = Buffer[Out];
		
		Out++;
		
		//handle wrap
		if(Out > MAX_ENTRIES-1)
		{
			Out = 0;
		}
		
		return true;
	}
	
	return false;
}

/* Empties the circular buffer. The old data is 
    still present, but iterators and count will 
    ignore it. */
  void Clear()
  {
    Size = 0;
	In = 0;
	Out = 0;
  }

/* Returns the number of entries that are stored
in the circular buffer. */
  int CountStored() const
  {
    return Size;
  }

/* Returns the maximum number of values that can
be stored in the circular buffer. */
  int MaxSize() const
  {
    return MAX_ENTRIES;
  }

};

