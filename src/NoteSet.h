// NoteSet.h
//
// Copyright 2012, 2018 Olivier Gillet, Kathryn Schaffer
//
// Original Author: Olivier Gillet (ol.gillet@gmail.com)
// Modifications/new interface:  Kathryn Schaffer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//  This software is based substantially on NoteStack.h by Olivier Gillet.
//  Variable names have been modified, comments have been added,
//  and the code has been simplified for pedagogical purposes (which may mean
//  it is less optimized and less attentive to best practices than the original).
//
//  This library keeps track of a sequence of MIDI notes played simultaneously,
//  using a linked list of structure objects.  The point is to enable
//  monosynth-like behavior when notes are released or retriggered.
//  Interface functions allow notes to be selected from the set
//  based on a user-selectable priority setting.
//
//  Important notes:
//    * this version has a maximum of 10 notes in the set of notes (10 fingers!)
//    * you must call the init function for the code to work.
//    * the init function requires specification of the note priority, which can be
//      NOTE_PRIORITY_LAST
//      NOTE_PRIORITY_LOW
//      NOTE_PRIORITY_HIGH
//

#ifndef NOTESET_H
#define NOTESET_H

#include <Arduino.h>

// For ease of code-reading, core parameters are set here with #define statements
// This is the total number of keys we will be able to track at once
#define SET_SIZE 10

//we will use this value (hex 0xff = 255) to signal that we have an empty/null
//note and should note play it
#define EMPTY_NOTE 0xff

//these are flags we can use to select how we want the notes to be sorted/prioritized
#define NOTE_PRIORITY_LAST 1
#define NOTE_PRIORITY_LOW 2
#define NOTE_PRIORITY_HIGH 3

// the set of notes will be an array of struct objects.  A struct
// is just a bundle of data values.  Here we define the kind of
// struct we will use to store notes.
struct NoteInfo {
    // the NoteInfo struct stores MIDI values for the note
    byte note;
    byte velocity;
    
    //... and it stores an index that says which note is next in the sequence
    //the index has values ranging from 1 to "capacity."
    //this is how we implement the "linked list" -
    //each element in the set knows which one comes next in order.
    byte next_idx;
};



//Define the NoteSet class, which stores and manipulates a set of
//NoteInfo objects.

class NoteSet {
public:
    //constructor and destructor (nothing happens for either)
    NoteSet() { }
    ~NoteSet() { }
    
    //initialization:  must be called before you use the NoteSet.
    void init(byte priority);

    //Add or remove a note
    void note_on(byte note, byte velocity);
    void note_off(byte note);

    //I (KS) am not entirely sure how these functions work (yet)
    //but I am retaining them because I think ultimately this is
    //useable for designing arpeggiation etc.
    NoteInfo& most_recent_note();
    NoteInfo& least_recent_note();
    NoteInfo& played_note(byte index);
    NoteInfo& sorted_note(byte index);
    NoteInfo& note(byte index);
    
    //this just returns an empty note
    NoteInfo& no_note();
    
    //This is the main algorithm that decides which note is the
    //right one to play.
    NoteInfo& note_by_priority(byte priority);
 
    //This is a KS addition that wraps note_by_priority and adds
    //an additional check so that we do not retrigger a note that
    //is already playing.
    NoteInfo& get_next_note();
    

    //some convenience functions for getting info:
    byte get_last(){return last_played;};
    byte get_size() { return size; }
    byte max_size() { return capacity; }

    
private:

    //used by the init function:
    void set_priority(byte priority){the_priority = priority; };
    
    //here are all the internal parameters the code uses
    byte capacity = SET_SIZE;  //no point in having more notes available than fingers!
    byte size;
    NoteInfo notes[SET_SIZE + 1];  // First element is an empty node, based on how linked lists work!
    byte root_idx;  // values range from 1 to capacity
    byte sorted_idx[SET_SIZE + 1];  // values range from 1 to capacity
    byte the_priority;
    byte last_played;
};



#endif  // NOTESET_H
