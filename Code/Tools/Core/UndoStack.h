#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_CORE_API IUndoableAction
    {
        friend class UndoStack;

    public:

        virtual ~IUndoableAction() = default;

    protected:

        virtual void Undo() = 0;
        virtual void Redo() = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API UndoStack
    {
    public:

        ~UndoStack();

        inline bool CanUndo() { return !m_recordedActions.empty(); }
        void Undo();
        inline bool CanRedo() { return !m_undoneActions.empty(); }
        void Redo();

        //-------------------------------------------------------------------------

        // Register a new action, this transfers ownership of the action memory to the stack
        void RegisterAction( IUndoableAction* pAction );

    private:

        void ClearUndoStack();
        void ClearRedoStack();

    private:

        TVector<IUndoableAction*>    m_recordedActions;
        TVector<IUndoableAction*>    m_undoneActions;
    };
}