/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//
// Copyright (C) 2010 V.R.Madgazin
// www.vmgames.com vrm@vmgames.com
//

#include "jdksmidi/world.h"
#include "jdksmidi/filewritemultitrack.h"

namespace jdksmidi
{

MIDIFileWriteMultiTrack::MIDIFileWriteMultiTrack (
    const MIDIMultiTrack *mlt_,
    MIDIFileWriteStream *strm_
    )
        :
        multitrack ( mlt_ ),
        writer ( strm_ )
{
}

MIDIFileWriteMultiTrack::~MIDIFileWriteMultiTrack()
{
}

bool MIDIFileWriteMultiTrack::Write ( int num_tracks, int division )
{
    bool f = true;

    if ( !PreWrite() )
    {
        return false;
    }

    // first, write the header.
    writer.WriteFileHeader ( ( num_tracks > 1 )? 1:0, num_tracks, division ); // VRM
    // now write each track

    for ( int i = 0; i < num_tracks; ++i )
    {
        if ( writer.ErrorOccurred() )
        {
            f = false;
            break;
        }

        const MIDITrack *t = multitrack->GetTrack ( i );

        if ( !t->EventsOrderOK() ) // VRM time of events out of order: t->SortEventsOrder() must be done externally
        {
            f = false;
            break;
        }

        writer.WriteTrackHeader ( 0 ); // will be rewritten later

        if ( t )
        {
            for ( int event_num = 0; event_num < t->GetNumEvents(); ++event_num )
            {
                const MIDITimedBigMessage *ev = t->GetEventAddress ( event_num );

                if ( ev && !ev->IsNoOp() )
                {
                    if ( !ev->IsDataEnd() )
                    {
                        writer.WriteEvent ( *ev );

                        if ( writer.ErrorOccurred() )
                        {
                            f = false;
                            break;
                        }
                    }
                }
            }
        }

        writer.WriteEndOfTrack ( 0 );
        writer.RewriteTrackLength();
    }

    if ( !PostWrite() )
    {
        return false;
    }

    return f;
}


bool MIDIFileWriteMultiTrack::PreWrite()
{
    return true;
}

bool MIDIFileWriteMultiTrack::PostWrite()
{
    return true;
}



}
