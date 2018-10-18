//==============================================================================
// Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file manages multiple instances of the ACL Module
//==============================================================================

#ifndef _WIN32
    #include <dlfcn.h>
#endif

#include "ACLModuleManager.h"

ACLModuleManager::ACLModuleManager() :
    m_pAclModule12(nullptr),
    m_pCompiler12(nullptr),
    m_pAclModule20(nullptr),
    m_pCompiler20(nullptr)
{
}

bool ACLModuleManager::GetACLModule(bool useHSAILPath, ACLModule*& pAclModule, aclCompiler*& pAclCompiler)
{
    bool retVal = false;

    if (useHSAILPath)
    {
        retVal = nullptr != m_pAclModule20;

        if (!retVal)
        {
            m_pAclModule20 = new(std::nothrow) ACLModule();

            if (nullptr != m_pAclModule20)
            {
                m_pAclModule20->UnloadModule();
                std::string moduleName(ACLModule::s_TMP_MODULE_NAME);
#ifndef _WIN32
                void* pModuleHandle = dlopen(moduleName.c_str(), RTLD_LAZY | RTLD_NOLOAD | RTLD_LOCAL);

                if (nullptr == pModuleHandle && nullptr != ACLModule::s_TMP_ORCA_MODULE_NAME)
                {
                    pModuleHandle = dlopen(ACLModule::s_TMP_ORCA_MODULE_NAME, RTLD_LAZY | RTLD_NOLOAD | RTLD_LOCAL);

                    if (nullptr != pModuleHandle)
                    {
                        moduleName = ACLModule::s_TMP_ORCA_MODULE_NAME;
                    }
                }
#endif
                m_pAclModule20->LoadModule(moduleName);

                if (m_pAclModule20->IsLoaded())
                {
                    m_pCompiler20 = m_pAclModule20->CompilerInit(NULL, NULL);
                    retVal = nullptr != m_pCompiler20;
                }
            }
        }

        if (retVal)
        {
            pAclModule = m_pAclModule20;
            pAclCompiler = m_pCompiler20;
            retVal = pAclModule->IsLoaded() && (nullptr != pAclCompiler);
        }
    }
    else
    {
        retVal = nullptr != m_pAclModule12;

        if (!retVal)
        {
            m_pAclModule12 = new(std::nothrow) ACLModule();

            if (nullptr != m_pAclModule12)
            {
                if (m_pAclModule12->IsLoaded())
                {
                    m_pCompiler12 = m_pAclModule12->CompilerInit(NULL, NULL);
                    retVal = nullptr != m_pCompiler12;
                }
            }
        }

        if (retVal)
        {
            pAclModule = m_pAclModule12;
            pAclCompiler = m_pCompiler12;
            retVal = pAclModule->IsLoaded() && (nullptr != pAclCompiler);
        }
    }

    return retVal;
}

void ACLModuleManager::UnloadAllACLModules()
{
    if (nullptr != m_pCompiler12 && nullptr != m_pAclModule12)
    {
        m_pAclModule12->CompilerFini(m_pCompiler12);
        delete m_pAclModule12;
    }

    if (nullptr != m_pCompiler20 && nullptr != m_pAclModule20)
    {
        m_pAclModule20->CompilerFini(m_pCompiler20);
        delete m_pAclModule20;
    }
}
