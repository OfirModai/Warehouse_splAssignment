#pragma once
#include <string>
#include <vector>
#include "WareHouse.h"
using std::string;
using std::vector;

enum class ActionStatus{
    COMPLETED, ERROR
};

enum class CustomerType{
    Soldier, Civilian
};


class Customer;

class BaseAction{
    public:
        BaseAction();
        ActionStatus getStatus() const;
        virtual void act(WareHouse& wareHouse);
        virtual string toString() const=0;
        virtual BaseAction* clone() const=0;

        ActionStatus getStatus() const{ return status;}
        BaseAction(){}
        virtual void act(WareHouse& wareHouse){
            wareHouse.addAction(this);
        }

    protected:
        void complete();
        void error(string errorMsg);
        string getErrorMsg() const;

        void complete(){status=ActionStatus::COMPLETED;}
        void error(string errorMsg){errorMsg=errorMsg; status = ActionStatus::ERROR;}
        string getErrorMsg() const{return errorMsg;}

    private:
        string errorMsg;
        ActionStatus status;
};

class SimulateStep : public BaseAction {

    public:
        SimulateStep(int numOfSteps);
        void act(WareHouse &wareHouse) override;
        std::string toString() const override;
        SimulateStep *clone() const override;

        SimulateStep(int numOfSteps):numOfSteps(numOfSteps){}
        void act(WareHouse &warehouse){
            BaseAction::act(warehouse);
            warehouse.SimulateStep();
            complete();
        }
        SimulateStep* clone() const override{return new SimulateStep(*this);}
        private:
        const int numOfSteps;
};

class AddOrder : public BaseAction {
    public:
        AddOrder(int id);
        void act(WareHouse &wareHouse) override;
        AddOrder *clone() const override;
        string toString() const override;

        AddOrder(int id): customerId(id) {}
        void act(WareHouse &wareHouse) override{
            BaseAction::act(wareHouse);
            string message = wareHouse.addOrder(customerId);
            if(message=="") complete();
            else {
                error(message);
                cout<< this->toString();
            }
        }
        string toString() const override{
            string res = "order" + customerId;
            if(getStatus()==ActionStatus::COMPLETED) res += "Completed";
            else res += "Error:" + getErrorMsg();
            return res;
        }
        AddOrder* clone() const override{return new AddOrder(*this);}
    private:
        const int customerId;
};


class AddCustomer : public BaseAction {
    public:
        AddCustomer(string customerName, string customerType, int distance, int maxOrders)
        : customerName(customerName), customerType(convertStringToCustomerType(customerType)),
         distance(distance), maxOrders(maxOrders){}
        void act(WareHouse &wareHouse) override{
            BaseAction::act(wareHouse);
            wareHouse.addCustomer(customerName,customerType,distance,maxOrders);
            complete();
        }
        AddCustomer *clone() const override{
            return new AddCustomer(*this);
        }
        string toString() const override{
            string anoying = "soldier";
            if (customerType == CustomerType::Civilian) anoying = "civilian";
            return "customer " + customerName + " " + anoying + " " +
             std::to_string(distance) + " " + std::to_string(maxOrders);
        }
    private:
        const string customerName;
        const CustomerType customerType;
        const int distance;
        const int maxOrders;

        CustomerType convertStringToCustomerType(string s){
            if(s=="civilian") return CustomerType::Civilian;
            else return CustomerType::Soldier;
        }
};



class PrintOrderStatus : public BaseAction {
    public:
        PrintOrderStatus(int id);
        void act(WareHouse &wareHouse) override;
        PrintOrderStatus *clone() const override;
        string toString() const override;
        PrintOrderStatus(int id):orderId(id){}
        void act(WareHouse &wareHouse) override{
            BaseAction::act(wareHouse);
            std::cout << wareHouse.getOrderStatus(orderId) << std::endl;
        }
    private:
        const int orderId;
};

class PrintCustomerStatus: public BaseAction {
    public:
        PrintCustomerStatus(int customerId);
        void act(WareHouse &wareHouse) override;
        PrintCustomerStatus *clone() const override;
        string toString() const override;

        
    private:
        const int customerId;
};


class PrintVolunteerStatus : public BaseAction {
    public:
        PrintVolunteerStatus(int id);
        void act(WareHouse &wareHouse) override;
        PrintVolunteerStatus *clone() const override;
        string toString() const override;
    private:
        const int VolunteerId;
};


class PrintActionsLog : public BaseAction {
    public:
        PrintActionsLog();
        void act(WareHouse &wareHouse) override;
        PrintActionsLog *clone() const override;
        string toString() const override;
    private:
};

class Close : public BaseAction {
    public:
        Close();
        Close *clone() const override;
        void act(WareHouse &wareHouse) override;
        string toString() const override;
    private:
};

class BackupWareHouse : public BaseAction {
    public:
        BackupWareHouse();
        void act(WareHouse &wareHouse) override;
        BackupWareHouse *clone() const override;
        string toString() const override;
    private:
};


class RestoreWareHouse : public BaseAction {
    public:
        RestoreWareHouse();
        void act(WareHouse &wareHouse) override;
        RestoreWareHouse *clone() const override;
        string toString() const override;
    private:
};