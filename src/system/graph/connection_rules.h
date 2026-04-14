#pragma once

#include <DecToolsBox/core/condition.hpp>
#include <system/obj/graph/base.h>
#include <set>
#include <vector>

class BaseConnectionRule : public BaseRule {
public:
    BaseConnectionRule();
    BaseConnectionRule(GraphBase* p_node_1, GraphBase* p_node_2);
    virtual ~BaseConnectionRule() = default;

    void set_nodes(GraphBase* p_node_1, GraphBase* p_node_2);
    bool has_valid_nodes() const;

    bool is_pass() const override = 0;

protected:
    GraphBase* m_node_1 = nullptr;
    GraphBase* m_node_2 = nullptr;
};

class ConnectionRuleTargetNotSelf : public BaseConnectionRule {
public:
    using BaseConnectionRule::BaseConnectionRule;

    bool is_pass() const override;
};

class ConnectionRuleTargetNotType : public BaseConnectionRule {
public:
    ConnectionRuleTargetNotType(
        GraphBase* p_node_1,
        GraphBase* p_node_2,
        GraphManager::NodeTypeId p_blocked_type
    );

    bool is_pass() const override;

private:
    GraphManager::NodeTypeId m_blocked_type;
};

class ConnectionRuleTargetNotAlreadyConnected : public BaseConnectionRule {
public:
    using BaseConnectionRule::BaseConnectionRule;

    bool is_pass() const override;
};

class ConnectionRuleTargetNotParentDirectly : public BaseConnectionRule {
public:
    using BaseConnectionRule::BaseConnectionRule;

    bool is_pass() const override;
};

class ConnectionRuleTargetNotParentProxyOfType : public BaseConnectionRule {
public:
    ConnectionRuleTargetNotParentProxyOfType(
        GraphBase* p_node_1,
        GraphBase* p_node_2,
        GraphManager::NodeTypeId p_fm_type
    );

    bool is_pass() const override;

private:
    GraphManager::NodeTypeId m_fm_type;
};

class ConnectionRuleToRepeaterNotHaveParent : public BaseConnectionRule {
public:
    using BaseConnectionRule::BaseConnectionRule;

    bool is_pass() const override;
};

class ConnectionRuleFmRepeaterNotHaveChildren : public BaseConnectionRule {
public:
    using BaseConnectionRule::BaseConnectionRule;

    bool is_pass() const override;
};

class ConnectionRuleFmNotConnectToTypeYet : public BaseConnectionRule {
public:
    ConnectionRuleFmNotConnectToTypeYet(
        GraphBase* p_node_1,
        GraphBase* p_node_2,
        GraphManager::NodeTypeId p_blocked_child_type
    );

    bool is_pass() const override;

private:
    GraphManager::NodeTypeId m_blocked_child_type;
};

class ConnectionRuleIfConnectedToOptionThenTargetMustBeOptionOrRepeater : public BaseConnectionRule {
public:
    using BaseConnectionRule::BaseConnectionRule;

    bool is_pass() const override;
};