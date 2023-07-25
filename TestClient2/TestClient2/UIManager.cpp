#include "UIManager.h"
#include "UI.h"

UIManager* UIManager::m_inst = nullptr;

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
    delete m_pUI;
}

bool UIManager::Init()
{
    if (!m_pUI)
        m_pUI = new UI();

    return true;
}
