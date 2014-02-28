#ifndef __RTT_GAZEBO_DEPLOYER_GAZEBO_DEPLOYER_MODEL_PLUGIN_H
#define __RTT_GAZEBO_DEPLOYER_GAZEBO_DEPLOYER_MODEL_PLUGIN_H

#include <cstdlib>

// Boost
#include <boost/bind.hpp>

// Gazebo
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>

// Orocos
#include <rtt/deployment/ComponentLoader.hpp>
#include <ocl/DeploymentComponent.hpp>
#include <ocl/TaskBrowser.hpp>
#include <ocl/LoggingService.hpp>
#include <rtt/Logger.hpp>


#include <rtt/os/startstop.h>
#include <rtt/scripting/Scripting.hpp>
#include <rtt/transports/corba/corba.h>
#include <rtt/transports/corba/TaskContextServer.hpp>

namespace rtt_gazebo_deployer {

  class GazeboDeployerModelPlugin : public gazebo::ModelPlugin
  {
  public:

    /* \brief Constructor
     * Gets an instance of \ref RTTSystem
     */
    GazeboDeployerModelPlugin();

    //! Destructor, disconnects update
    virtual ~GazeboDeployerModelPlugin();

    /**
     * \brief Gazebo entry point
     *
     * This ensures that the "gazebo" root deployer is created, then it creates a
     * deployer specific for this gazebo model if desired.
     *
     * Then it optionally loades Orocos script from <opsScriptFile> or
     * <opsScript> tags in the SDF. Once these scripts have been run, it
     * is presumed that the desired RTT components are all loaded. If a
     * component is meant to be connected to the Gazebo world update for
     * this model, given by the <component> tag in the SDF, this is where
     * it is connected.
     *
     * The component's gazebo.configure() operation is called, and then an
     * event connection is registered with Gazebo to call the component's
     * gazebo.update() operation whenever Gazebo's world updates.
     */
    void Load(gazebo::physics::ModelPtr parent, sdf::ElementPtr sdf);

    //! Continuation of Load() in seperate thread from Gazebo in case something blocks
    void loadThread();

    //! Called by the world update start event
    void gazeboUpdate();

  private:
    
    //! Server for connecting to the deployer over CORBA
    static RTT::corba::TaskContextServer * taskcontext_server;
    //! Map between model names and deployment components
    static std::map<std::string,boost::shared_ptr<OCL::DeploymentComponent> > deployers;

    //! Shared pointer to the RTTSystem singleton
    boost::shared_ptr<RTTSystem> rtt_system_;

    //! Deferred load in case something blocks
    boost::thread deferred_load_thread_;

    //! The parent model
    gazebo::physics::ModelPtr parent_model_;

    //! The SDF for this model
    sdf::ElementPtr sdf_;

    //! A Gazebo event connection to the world update
    gazebo::event::ConnectionPtr update_connection_;

    //! The name of the deployer in which this model should be loaded
    std::string deployer_name_;

    //! The name of the RTT component loaded in the deployer which should be connected to gazebo
    RTT::TaskContext* model_component_;

    //! Operation for polling the model component 
    RTT::OperationCaller<void(gazebo::physics::ModelPtr)> gazebo_update_;
  };
}

#endif // ifndef __RTT_GAZEBO_DEPLOYER_GAZEBO_DEPLOYER_MODEL_PLUGIN_H
