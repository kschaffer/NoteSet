//NoteSet.cpp
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

#include "NoteSet.h"

void NoteSet::init(byte priority) {
    //the first two lines here are a KS addition to allow for
    //the get_next_note functionality
    set_priority(priority);
    last_played = EMPTY_NOTE;
    
    
    size = 0;
    //this set all bytes that we reserved for our note set to zero
    memset(notes + 1, 0, sizeof(NoteInfo) * capacity);
    //this does the same thing for the sorted list of note indices
    memset(sorted_idx + 1, 0, capacity);
    //set the index of the "root" note to zero
    root_idx = 0;
    //go through every note in the set and set it to "empty"
    for (byte i = 0; i <= capacity; ++i) {
        notes[i].note = EMPTY_NOTE;
    }
}



void NoteSet::note_on(byte note, byte velocity) {
    // Remove the note from the list first (in case it is already here).
    note_off(note);
    
    //If the whole set of notes is already full, then we bump out the least
    //recently played note to make room for this one.
    if (size == capacity) {
        byte least_recent_note = 1;
        //loop through and check which note is at the end of the
        //list, meaning its "next_idx" points to nothing (0)
        for (byte i = 1; i <= capacity; ++i) {
            if (notes[i].next_idx == 0) {
                least_recent_note = notes[i].note;
            }
        }
        note_off(least_recent_note);
    }
    // Now we are ready to insert the new note. Find a free slot to insert it.
    byte free_slot = 1;
    //loop through and check for a place we can stick this note.
    for (byte i = 1; i <= capacity; ++i) {
        if (notes[i].note == EMPTY_NOTE) {
            free_slot = i;
            break;
        }
    }
   
    //note that root_idx is initialized to zero in the init()
    //function.
    notes[free_slot].next_idx = root_idx;
    notes[free_slot].note = note;
    notes[free_slot].velocity = velocity;
    //update the "root" index to be the last note we filled
    root_idx = free_slot;
    
    // The last step consists in inserting the note in the sorted list.
    for (byte i = 0; i < size; ++i) {
        if (notes[sorted_idx[i]].note > note) {
            for (byte j = size; j > i; --j) {
                sorted_idx[j] = sorted_idx[j - 1];
            }
            sorted_idx[i] = free_slot;
            free_slot = 0;
            break;
        }
    }
    if (free_slot) {
        sorted_idx[size] = free_slot;
    }
    ++size;
}

void NoteSet::note_off(byte note) {
    //all this basically just removes the note
    //from the set and updates the linkages and
    //sorted lists
    byte current = root_idx;
    byte previous = 0;
    while (current) {
        if (notes[current].note == note) {
            break;
        }
        previous = current;
        current = notes[current].next_idx;
    }
    if (current) {
        if (previous) {
            notes[previous].next_idx = notes[current].next_idx;
        } else {
            root_idx = notes[current].next_idx;
        }
        for (byte i = 0; i < size; ++i) {
            if (sorted_idx[i] == current) {
                for (byte j = i; j < size - 1; ++j) {
                    sorted_idx[j] = sorted_idx[j + 1];
                }
                break;
            }
        }
        notes[current].next_idx = 0;
        notes[current].note = EMPTY_NOTE;
        notes[current].velocity = 0;
        --size;
    }
    //this is a KS addition to allow for the functionality
    //of get_next_note
    if (note == last_played){
        last_played = EMPTY_NOTE;
    }
}


NoteInfo& NoteSet::most_recent_note() {
    return notes[root_idx];
    
}

NoteInfo& NoteSet::least_recent_note() {
    byte current = root_idx;
    while (current && notes[current].next_idx) {
        current = notes[current].next_idx;
    }
    return notes[current];
}

NoteInfo& NoteSet::played_note(byte index) {
    byte current = root_idx;
    index = size - index - 1;
    for (byte i = 0; i < index; ++i) {
        current = notes[current].next_idx;
    }
    return notes[current];
}

NoteInfo& NoteSet::sorted_note(byte index)  {
    return notes[sorted_idx[index]];
}

NoteInfo& NoteSet::note(byte index) {
    return notes[index];
}



NoteInfo& NoteSet::no_note()  {
    //notes[0] is a null struct (nothing in it)
    //the code using this code should test for this case and
    //play nothing
        return notes[0];
    
}



NoteInfo& NoteSet::note_by_priority(byte priority) {
    //This is the main function that picks which note
    //should be the one to play
    if (get_size() == 0) {
        return no_note();
    }
    switch (priority) {
        case NOTE_PRIORITY_LAST:
            return most_recent_note();
            
        case NOTE_PRIORITY_LOW:
            return sorted_note(0);
            
        case NOTE_PRIORITY_HIGH:
            return sorted_note(get_size() - 1);
            
        default:
            return no_note();
    }
}

NoteInfo& NoteSet::get_next_note(){
    //This is a KS addition that wraps note_by_priority and adds
    //an additional check to avoid retriggering a note that is
    //already playing.
    
    NoteInfo& note_to_play = note_by_priority(the_priority);
    
    if ((note_to_play.note != EMPTY_NOTE)&&(note_to_play.note != last_played)){
        
        last_played = note_to_play.note;
        return note_to_play;
    } else{
        return no_note();
    }
    
    
}
