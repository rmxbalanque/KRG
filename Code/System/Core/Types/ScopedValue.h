#pragma once

#include "System/Core/Core.h"

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T>
    class TScopedValue
    {
    public:

        TScopedValue( T& variable, T const& newValue )
            : m_variable( variable )
            , m_originalValue( variable )

        {
            variable = newValue;
        }

        ~TScopedValue()
        {
            m_variable = m_originalValue;
        }

    private:

        TScopedValue( TScopedValue const& ) = delete;
        TScopedValue( TScopedValue const&& ) = delete;
        TScopedValue& operator=( TScopedValue const& ) = delete;
        TScopedValue& operator=( TScopedValue const&& ) = delete;

    private:

        T&  m_variable;
        T   m_originalValue;
    };
}