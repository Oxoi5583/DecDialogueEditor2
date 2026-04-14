#include "connection_rules.h"

BaseConnectionRule::BaseConnectionRule() = default;

BaseConnectionRule::BaseConnectionRule(GraphBase* p_node_1, GraphBase* p_node_2)
    : m_node_1(p_node_1), m_node_2(p_node_2) {}

void BaseConnectionRule::set_nodes(GraphBase* p_node_1, GraphBase* p_node_2){
    m_node_1 = p_node_1;
    m_node_2 = p_node_2;
}

bool BaseConnectionRule::has_valid_nodes() const{
    return m_node_1 != nullptr && m_node_2 != nullptr;
}

bool ConnectionRuleTargetNotSelf::is_pass() const{
    if(!has_valid_nodes()) return false;
    return m_node_1->get_id() != m_node_2->get_id();
}

ConnectionRuleTargetNotType::ConnectionRuleTargetNotType(
    GraphBase* p_node_1,
    GraphBase* p_node_2,
    GraphManager::NodeTypeId p_blocked_type
)
    : BaseConnectionRule(p_node_1, p_node_2),
      m_blocked_type(p_blocked_type) {}

bool ConnectionRuleTargetNotType::is_pass() const{
    if(!has_valid_nodes()) return false;
    return m_node_2->get_type() != m_blocked_type;
}

bool ConnectionRuleTargetNotAlreadyConnected::is_pass() const{
    if(!has_valid_nodes()) return false;

    std::vector<OID> ancestor_ids = m_node_1->skip_from_repeater();
    for(const OID& ancestor_id : ancestor_ids){
        GraphBase* ancestor = ObjectServer::Ref()->get_instance<GraphBase>(ancestor_id);
        if(ancestor == nullptr) continue;

        std::set<OID> children = ancestor->get_children_set(true);
        if(children.contains(m_node_2->get_id())){
            return false;
        }
    }

    return true;
}

bool ConnectionRuleTargetNotParentDirectly::is_pass() const{
    if(!has_valid_nodes()) return false;

    std::set<OID> ancestor_ids = m_node_1->get_parent_set(false);
    return !ancestor_ids.contains(m_node_2->get_id());
}

ConnectionRuleTargetNotParentProxyOfType::ConnectionRuleTargetNotParentProxyOfType(
    GraphBase* p_node_1,
    GraphBase* p_node_2,
    GraphManager::NodeTypeId p_fm_type
)
    : BaseConnectionRule(p_node_1, p_node_2),
      m_fm_type(p_fm_type) {}

bool ConnectionRuleTargetNotParentProxyOfType::is_pass() const{
    if(!has_valid_nodes()) return false;

    std::set<OID> ancestor_ids = m_node_1->get_parent_set(true, false);

    if(ancestor_ids.contains(m_node_2->get_id()) &&
       m_node_1->get_type() == m_fm_type){
        return false;
    }

    return true;
}

bool ConnectionRuleToRepeaterNotHaveParent::is_pass() const{
    if(!has_valid_nodes()) return false;

    if(m_node_2->get_type() == GraphManager::REPEATER){
        std::set<OID> parent = m_node_2->get_parent_set();
        return parent.empty();
    }

    return true;
}

bool ConnectionRuleFmRepeaterNotHaveChildren::is_pass() const{
    if(!has_valid_nodes()) return false;

    if(m_node_1->get_type() == GraphManager::REPEATER){
        std::set<OID> children = m_node_1->get_children_set();
        return children.empty();
    }

    return true;
}

ConnectionRuleFmNotConnectToTypeYet::ConnectionRuleFmNotConnectToTypeYet(
    GraphBase* p_node_1,
    GraphBase* p_node_2,
    GraphManager::NodeTypeId p_blocked_child_type
)
    : BaseConnectionRule(p_node_1, p_node_2),
      m_blocked_child_type(p_blocked_child_type) {}

bool ConnectionRuleFmNotConnectToTypeYet::is_pass() const{
    if(!has_valid_nodes()) return false;

    auto children = m_node_1->get_children(true);
    for(OID id : children){
        GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(obj == nullptr) continue;

        if(obj->get_type() == m_blocked_child_type){
            return false;
        }
    }

    return true;
}

bool ConnectionRuleIfConnectedToOptionThenTargetMustBeOptionOrRepeater::is_pass() const{
    if(!has_valid_nodes()) return false;

    bool is_connected_to_option = false;

    auto children = m_node_1->get_children(true);
    for(OID id : children){
        GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(obj == nullptr) continue;

        if(obj->get_type() == GraphManager::OPTION){
            is_connected_to_option = true;
            break;
        }
    }

    if(is_connected_to_option){
        GraphManager::NodeTypeId to_type = m_node_2->get_type();
        if(to_type != GraphManager::OPTION &&
           to_type != GraphManager::REPEATER){
            return false;
        }
    }

    return true;
}