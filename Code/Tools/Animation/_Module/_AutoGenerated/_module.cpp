//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Tools\Animation\_Module\Module.h"
#include "System/Resource/ResourceSystem.h"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\1236551654.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\1307461330.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\1314544678.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\139288460.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\1551038611.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\2174867705.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\2297194835.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\2442786634.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\246030321.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\2677063852.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\277042631.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\3305565781.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\3367762967.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\3446627217.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\3713120435.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\4034344745.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\706285501.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\730365719.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\755347380.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\757617462.cpp"
#include "D:\Kruger\Code\Tools\Animation\_Module\_AutoGenerated\870748305.cpp"

//-------------------------------------------------------------------------

void KRG::Animation::ToolsModule::RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::FlowToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateLayerDataToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateToolsNode::TimedStateEvent>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstBoolToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstIDToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstIntToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstFloatToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstVectorToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstTargetToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::EntryStateOverrideConditionsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::FlowToolGraph>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::EntryStateOverrideGraph>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::EntryStateOverrideNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ResultToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateMachineToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::TransitionToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::GlobalTransitionToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::TransitionConduitToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::GlobalTransitionsToolsGraph>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::GlobalTransitionsToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterReferenceToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterReferenceToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ZeroPoseToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ReferencePoseToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::DataSlotToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AnimationPoseToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventItem>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventTrack>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationClipResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::SkeletonResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateMachineToolsGraph>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AnimationClipToolsNode>::RegisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_Animation_Graph_GraphType::RegisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationGraphVariationResourceDescriptor>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::SelectorConditionNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::SelectorToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AndToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::OrToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::NotToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::DataSlotToolsNode::OverrideValue>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Variation>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ParameterizedBlendToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::RangedBlendToolsNode>::RegisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VelocityBlendToolsNode>::RegisterType( typeRegistry );
}

void KRG::Animation::ToolsModule::UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )
{
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VelocityBlendToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::RangedBlendToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ParameterizedBlendToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Variation>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::DataSlotToolsNode::OverrideValue>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::NotToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::OrToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AndToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::SelectorToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::SelectorConditionNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationGraphVariationResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::EnumHelpers::EnumHelper_KRG_Animation_Graph_GraphType::UnregisterEnum( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AnimationClipToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateMachineToolsGraph>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::SkeletonResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationClipResourceDescriptor>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventTrack>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventItem>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AnimationPoseToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::DataSlotToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ReferencePoseToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ZeroPoseToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterReferenceToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterReferenceToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::GlobalTransitionsToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::GlobalTransitionsToolsGraph>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::TransitionConduitToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::GlobalTransitionToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::TransitionToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateMachineToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ResultToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::EntryStateOverrideNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::EntryStateOverrideGraph>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::FlowToolGraph>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::EntryStateOverrideConditionsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstTargetToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstVectorToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstFloatToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstIntToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstIDToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ConstBoolToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateToolsNode::TimedStateEvent>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::StateLayerDataToolsNode>::UnregisterType( typeRegistry );
    TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::FlowToolsNode>::UnregisterType( typeRegistry );
}

