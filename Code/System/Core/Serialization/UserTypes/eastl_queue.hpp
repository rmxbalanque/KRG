/*! \file queue.hpp
    \brief Support for types found in \<queue\>
    \ingroup STLSupport */
/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_QUEUE_HPP_
#define CEREAL_TYPES_QUEUE_HPP_

#include <cereal/details/helpers.hpp>
#include <EASTL/queue.h>
#include <EASTL/priority_queue.h>

namespace cereal
{
  namespace queue_detail
  {
    //! Allows access to the protected container in queue
    /*! @internal */
    template <class T, class C> inline
    C const & container( eastl::queue<T, C> const & queue )
    {
      struct H : public eastl::queue<T, C>
      {
        static C const & get( eastl::queue<T, C> const & q )
        {
          return q.*(&H::c);
        }
      };

      return H::get( queue );
    }

    //! Allows access to the protected container in priority queue
    /*! @internal */
    template <class T, class C, class Comp> inline
    C const & container( eastl::priority_queue<T, C, Comp> const & priority_queue )
    {
      struct H : public eastl::priority_queue<T, C, Comp>
      {
        static C const & get( eastl::priority_queue<T, C, Comp> const & pq )
        {
          return pq.*(&H::c);
        }
      };

      return H::get( priority_queue );
    }

    //! Allows access to the protected comparator in priority queue
    /*! @internal */
    template <class T, class C, class Comp> inline
    Comp const & comparator( eastl::priority_queue<T, C, Comp> const & priority_queue )
    {
      struct H : public eastl::priority_queue<T, C, Comp>
      {
        static Comp const & get( eastl::priority_queue<T, C, Comp> const & pq )
        {
          return pq.*(&H::comp);
        }
      };

      return H::get( priority_queue );
    }
  }

  //! Saving for eastl::queue
  template <class Archive, class T, class C> inline
  void CEREAL_SAVE_FUNCTION_NAME( Archive & ar, eastl::queue<T, C> const & queue )
  {
    ar( CEREAL_NVP_("container", queue_detail::container( queue )) );
  }

  //! Loading for eastl::queue
  template <class Archive, class T, class C> inline
  void CEREAL_LOAD_FUNCTION_NAME( Archive & ar, eastl::queue<T, C> & queue )
  {
    C container;
    ar( CEREAL_NVP_("container", container) );
    queue = eastl::queue<T, C>( eastl::move( container ) );
  }

  //! Saving for eastl::priority_queue
  template <class Archive, class T, class C, class Comp> inline
  void CEREAL_SAVE_FUNCTION_NAME( Archive & ar, eastl::priority_queue<T, C, Comp> const & priority_queue )
  {
    ar( CEREAL_NVP_("comparator", queue_detail::comparator( priority_queue )) );
    ar( CEREAL_NVP_("container", queue_detail::container( priority_queue )) );
  }

  //! Loading for eastl::priority_queue
  template <class Archive, class T, class C, class Comp> inline
  void CEREAL_LOAD_FUNCTION_NAME( Archive & ar, eastl::priority_queue<T, C, Comp> & priority_queue )
  {
    Comp comparator;
    ar( CEREAL_NVP_("comparator", comparator) );

    C container;
    ar( CEREAL_NVP_("container", container) );

    priority_queue = eastl::priority_queue<T, C, Comp>( comparator, eastl::move( container ) );
  }
} // namespace cereal

#endif // CEREAL_TYPES_QUEUE_HPP_
