/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_ACTION_H
#define YARP_ROBOTINTERFACE_ACTION_H

#include <yarp/robotinterface/Types.h>

namespace yarp {
namespace robotinterface {

class YARP_robotinterface_API Action
{
public:
    explicit Action();
    Action(ActionPhase phase, ActionType type, unsigned int level);
    Action(const std::string &phase, const std::string &type, unsigned int level);
    Action(const Action &other);
    Action& operator=(const Action &other);

    ActionPhase& phase();
    ActionType& type();
    unsigned int& level();
    ParamList& params();

    ActionPhase phase() const;
    ActionType type() const;
    unsigned int level() const;
    const ParamList& params() const;

    bool hasParam(const std::string &name) const;
    std::string findParam(const std::string &name) const;

    virtual ~Action();

private:
    class Private;
    Private * const mPriv;
}; // class Action

} // namespace robotinterface
} // namespace yarp


namespace std { YARP_robotinterface_API std::ostream& operator<<(std::ostream &oss, const yarp::robotinterface::Action &t); }
YARP_robotinterface_API yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Action &t);


#endif // YARP_ROBOTINTERFACE_ACTION_H
