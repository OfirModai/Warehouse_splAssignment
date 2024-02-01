#include "../include/Action.h"

    ActionStatus BaseAction::getStatus() const { return status; };
    BaseAction::BaseAction() : errorMsg("didn't act yet"), status(ActionStatus::ERROR) {}
    void BaseAction::act(WareHouse &wareHouse)
    {
        if(errorMsg=="didn't act yet") errorMsg = "";
        wareHouse.addAction(this);
        if (status != ActionStatus::COMPLETED)
            cout<< this->toString() << std::endl;
    }
    string BaseAction::toString() const
    {
        if (status == ActionStatus::COMPLETED)
            return "COMPLETED";
        return "ERROR";
    }

    void BaseAction::complete() { status = ActionStatus::COMPLETED; }
    void BaseAction::error(string errorMsg)
    {
        this->errorMsg = errorMsg;
        status = ActionStatus::ERROR;
    }
    string BaseAction::getErrorMsg() const { return errorMsg; }
    CustomerType BaseAction::convertStringToCustomerType(string s)
    {
        if (s == "civilian")
            return CustomerType::Civilian;
        else
            return CustomerType::Soldier;
    }
    string BaseAction::convertCustomerTypeToString(CustomerType s)
    {
        if (s == CustomerType::Civilian)
            return "civilian";
        else
            return "soldier";
    }

    BaseAction::~BaseAction(){};

    SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}
    void SimulateStep::act(WareHouse &warehouse)
    {
        for (int i = 0; i < numOfSteps; i++)
            warehouse.simulateStep();
        complete();
        BaseAction::act(warehouse);
    }
    SimulateStep *SimulateStep::clone() const { return new SimulateStep(*this); }
    std::string SimulateStep::toString() const
    {
        return "simulateStep " + std::to_string(numOfSteps) + " " + BaseAction::toString();
    }


    AddOrder::AddOrder(int id) : customerId(id) {}
    void AddOrder::act(WareHouse &wareHouse) 
    {
        string message = wareHouse.addOrder(customerId);
        if (message == "") complete();
        else{
            error(message);
            cout << "ERROR: "+ message << std::endl;
        }
        wareHouse.addAction(this);
    }
    std::string AddOrder::toString() const 
    {
        return "order " + std::to_string(customerId) + " " + BaseAction::toString();
    }
    AddOrder *AddOrder::clone() const { return new AddOrder(*this); }

    AddCustomer::AddCustomer(string customerName, string customerType, int distance, int maxOrders)
        : customerName(customerName), customerType(BaseAction::convertStringToCustomerType(customerType)),
          distance(distance), maxOrders(maxOrders) {}
    void AddCustomer::act(WareHouse &wareHouse) 
    {
        wareHouse.addCustomer(customerName, BaseAction::convertCustomerTypeToString(customerType), distance, maxOrders);
        complete();
        BaseAction::act(wareHouse);
    }
    AddCustomer *AddCustomer::clone() const 
    {
        return new AddCustomer(*this);
    }
    string AddCustomer::toString() const 
    {
        string annoying = "soldier";
        if (customerType == CustomerType::Civilian)
            annoying = "civilian";
        return "customer " + customerName + " " + annoying + " " +
               std::to_string(distance) + " " + std::to_string(maxOrders) + " " + BaseAction::toString();
    }

    
    PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
    void PrintOrderStatus::act(WareHouse &wareHouse) 
    {
        string s = wareHouse.getOrderStatus(orderId);
        if (s == "Order doesn't exist")
            error(s);
        else
            complete();
        cout<< s << std::endl;
        BaseAction::act(wareHouse);
    }
    PrintOrderStatus *PrintOrderStatus::clone() const 
    {
        return new PrintOrderStatus(*this);
    }
    string PrintOrderStatus::toString() const 
    {
        return "orderStatus: " + orderId + BaseAction::toString();
    }


    PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}
    void PrintCustomerStatus::act(WareHouse &wareHouse) 
    {
        string s = wareHouse.getCustomerStatus(customerId);
        if (s == "Customer doesn't exist")
            error(s);
        else
            complete();
        cout<< s << std::endl;
        BaseAction::act(wareHouse);
    }
    PrintCustomerStatus *PrintCustomerStatus::clone() const 
    {
        return new PrintCustomerStatus(*this);
    }
    string PrintCustomerStatus::toString() const 
    {
        return "customerStatus: "+ customerId + BaseAction::toString();
    }

    PrintVolunteerStatus::PrintVolunteerStatus(int id) : VolunteerId(id) {}
    void PrintVolunteerStatus::act(WareHouse &wareHouse) 
    {
        string s = wareHouse.getVolunteerStatus(VolunteerId);
        if (s == "Volunteer doesn't exist")
            error(s);
        else
            complete();
        cout<< s << std::endl;
        BaseAction::act(wareHouse);
    }
    PrintVolunteerStatus *PrintVolunteerStatus::clone() const 
    {
        return new PrintVolunteerStatus(*this);
    }
    string PrintVolunteerStatus::toString() const 
    {
        return "volunteerStatus "+ VolunteerId + BaseAction::toString();
    }

    PrintActionsLog::PrintActionsLog(){};
    void PrintActionsLog::act(WareHouse &wareHouse) 
    {
        string s = wareHouse.getActionsLog();
        complete();
        std::cout << s << std::endl;
        BaseAction::act(wareHouse);
    }
    PrintActionsLog *PrintActionsLog::clone() const 
    {
        return new PrintActionsLog(*this);
    }
    string PrintActionsLog::toString() const 
    {
        return "log: " + BaseAction::toString();
    }


    Close::Close(){};
    Close *Close::clone() const { return new Close(*this); }
    string Close::toString() const { return "close " + BaseAction::toString(); }
    void Close::act(WareHouse &wareHouse)
    {
        BaseAction::act(wareHouse);
        wareHouse.close();
        complete();
        std::cout << "end program" << std::endl;
    }


    BackupWareHouse::BackupWareHouse(){};
    void BackupWareHouse::act(WareHouse &wareHouse) 
    {
        if (backup != nullptr)
            delete backup;
        backup = new WareHouse(wareHouse);
        complete();
        BaseAction::act(wareHouse);
    }
    BackupWareHouse *BackupWareHouse::clone() const
    {
        return new BackupWareHouse(*this);
    }
    string BackupWareHouse::toString() const 
    {
        return "backupWarehouse " + BaseAction::toString();
    }


    RestoreWareHouse::RestoreWareHouse(){};
    void RestoreWareHouse::act(WareHouse &wareHouse) 
    {

        if (backup == nullptr)
        {
            error("No Backup availble");
        }
        else
        {
            wareHouse = *backup;
            complete();
        }
        BaseAction::act(wareHouse);
    }
    RestoreWareHouse *RestoreWareHouse::clone() const 
    {
        return new RestoreWareHouse(*this);
    }
    string RestoreWareHouse::toString() const 
    {
        return "restoreWareHouse " + BaseAction::toString();
    }
