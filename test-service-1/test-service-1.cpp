/*
    Seup mesos dev
        sudo apt-get update
        sudo apt-get -y install tar wget git openjdk-8-jdk autoconf libtool build-essential python-dev libcurl4-nss-dev libsasl2-dev libsasl2-modules maven libapr1-dev libsvn-dev zlib1g-de
    
        mkdir mesos-src && cd mesos-src
        git clone https://git-wip-us.apache.org/repos/asf/mesos.git ./
        
        cd build
        mkdir /var/lib/mesos
        sudo ./bin/mesos-master.sh --ip=127.0.0.1 --work_dir=/var/lib/mesos
        sudo ./bin/mesos-agent.sh --master=127.0.0.1:5050 --work_dir=/var/lib/mesos
        
        Open page http://127.0.0.1:5050

    Make test-service-1
        sudo aptitude install libboost-all-dev
        sudo aptitude install libprotobuf-dev
        sudo aptitude install protobuf-compiler
    
        mkdir test-service-1 && cd test-service-1
        export MESOS_SRC=../../mesos-src
        export MESOS_LIBS=../../mesos-src/build/src/.libs
        make
        
    Run
        export LD_LIBRARY_PATH=$MESOS_LIBS
        ./test-service-1.bin
*/

#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <vector>
#include <mesos/resources.hpp>
#include <mesos/scheduler.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;

const char* SERVICE_NAME = "Test Service 1";
const char* MASTER_ADDR = "127.0.0.1:5050";
const float CPUS_PER_TASK = 1.0;
const int32_t MEM_PER_TASK = 32;

mesos::MesosSchedulerDriver* schedulerDriver;

class TestService : public mesos::Scheduler {
public:
    TestService() {
        cout << SERVICE_NAME << " :: construct" << endl;
    }

    virtual ~TestService() {}

    virtual void registered(mesos::SchedulerDriver*, const mesos::FrameworkID&, const mesos::MasterInfo&) {
        cout << SERVICE_NAME << " :: registered" << endl;
    }
    
    virtual void reregistered(mesos::SchedulerDriver*, const mesos::MasterInfo& masterInfo) {
        cout << SERVICE_NAME << " :: reregistered" << endl;
    }
    
    virtual void disconnected(mesos::SchedulerDriver* driver) {
        cout << SERVICE_NAME << " :: disconnected" << endl;
    }

    virtual void resourceOffers(mesos::SchedulerDriver* driver, const vector<mesos::Offer>& offers) {
        cout << SERVICE_NAME << " :: resourceOffers" << endl;
        
        cout << "Total resources: " << offers.size() << endl;
        if(offers.size() > 1) {
            const mesos::Offer& offer = offers[0];
            static mesos::Resources TASK_RESOURCES = mesos::Resources::parse(
                "cpus:" + stringify<float>(CPUS_PER_TASK) +
                ";mem:" + stringify<size_t>(MEM_PER_TASK)
            ).get();
            
            vector<mesos::TaskInfo> tasks;            
            mesos::TaskInfo task;
            task.set_name("Test task 1");
            tasks.push_back(task);
            
            driver->launchTasks(offer.id(), tasks);         
        }
    }

    virtual void offerRescinded(mesos::SchedulerDriver* driver, const mesos::OfferID& offerId) {
        cout << SERVICE_NAME << " :: offerRescinded" << endl;
    }

    virtual void statusUpdate(mesos::SchedulerDriver* driver, const mesos::TaskStatus& status) {
        cout << SERVICE_NAME << " :: statusUpdate" << endl;
    }

    virtual void frameworkMessage(mesos::SchedulerDriver* driver, const mesos::ExecutorID& executorId, const mesos::SlaveID& slaveId, const string& data) {
        cout << SERVICE_NAME << " :: frameworkMessage" << endl;
    }

    virtual void slaveLost(mesos::SchedulerDriver* driver, const mesos::SlaveID& sid) {
        cout << SERVICE_NAME << " :: slaveLost" << endl;
    }

    virtual void executorLost(mesos::SchedulerDriver* driver, const mesos::ExecutorID& executorID, const mesos::SlaveID& slaveID, int status) {
        cout << SERVICE_NAME << " :: executorLost" << endl;
    }

    virtual void error(mesos::SchedulerDriver* driver, const string& message) {
        cout << SERVICE_NAME << " :: error: " << message << endl;
    }
};

static void SIGINTHandler(int signum)
{
    if(schedulerDriver != NULL) {
        schedulerDriver->stop();
    }
    delete schedulerDriver;
    exit(0);
}

int main() {
    
    cout << SERVICE_NAME << " start " << endl;
    
    struct sigaction action;
    action.sa_handler = SIGINTHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    
    TestService scheduler;
    
    mesos::FrameworkInfo framework;
    framework.set_user("");
    framework.set_name(SERVICE_NAME);
    
    schedulerDriver = new mesos::MesosSchedulerDriver(&scheduler, framework, MASTER_ADDR);

    int status = schedulerDriver->run() == mesos::DRIVER_STOPPED ? 0 : 1;

    schedulerDriver->stop();

    delete schedulerDriver;
    
    cout << SERVICE_NAME << " end " << endl;
    
    return status;
}
