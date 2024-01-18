#pragma once
#include <string>
#include <vector>
#include "Order.h"
#include <stdexcept>
using std::string;
using std::vector;

#define NO_ORDER -1

class Volunteer {
    public:
        Volunteer(int id, const string& name);
        int getId() const;
        const string &getName() const;
        void resetActiveOrderId();
        void resetCompletedOrderId();
        int getActiveOrderId() const;
        int getCompletedOrderId() const;
        bool isBusy() const; // Signal whether the volunteer is currently processing an order    
        virtual bool hasOrdersLeft() const = 0; // Signal whether the volunteer didn't reach orders limit,Always true for CollectorVolunteer and DriverVolunteer
        virtual bool canTakeOrder(const Order &order) const = 0; // Signal if the volunteer can take the order.      
        virtual void acceptOrder(const Order &order) = 0; // Prepare for new order(Reset activeOrderId,TimeLeft,DistanceLeft,OrdersLeft depends on the volunteer type)
        virtual void step() = 0; //Simulate volunteer step,if the volunteer finished the order, transfer activeOrderId to completedOrderId

        virtual string toString() const = 0;
        virtual Volunteer* clone() const = 0; //Return a copy of the volunteer

        Volunteer(int id, const string& name)
        : id(id), name(name), completedOrderId(NO_ORDER), activeOrderId(NO_ORDER) {}
        int getId() const{ return id; };
        const string &getName() const{ 
            const string &ref_name = name;
            return ref_name;
        }; // is it the same as to just return name?
        int getActiveOrderId() const{ return activeOrderId; }
        int getCompletedOrderId() const{ return completedOrderId; }
        bool isBusy() const{ return getActiveOrderId()!= NO_ORDER; }
        virtual string toString() const{
            std::string res = "VolunteerId: " + std::to_string(getId()) +
                        "\n IsBusy: " + std::to_string(isBusy());
            if(isBusy()){
                res += "\n OrderId: " + std::to_string(getActiveOrderId());
            }
            return res;
        }

    protected:
        int completedOrderId; //Initialized to NO_ORDER if no order has been completed yet
        int activeOrderId; //Initialized to NO_ORDER if no order is being processed
        void resetActiveOrderId() {activeOrderId = NO_ORDER;}
        void resetCompletedOrderId() {completedOrderId = NO_ORDER;}
    
    private:
        const int id;
        const string name;

};


class CollectorVolunteer: public Volunteer {

    public:
        CollectorVolunteer(int id, string name, int coolDown);
        CollectorVolunteer *clone() const override;
        void step() override;
        int getCoolDown() const;
        int getTimeLeft() const;
        bool decreaseTimeLeft();//Decrease timeLeft by 1,return true if timeLeft=0,false otherwise
        // we changed the signature 
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override;
        void acceptOrder(const Order &order) override;
        string toString() const override;

        CollectorVolunteer(int id, string name, int coolDown)
        :Volunteer(id,name), coolDown(coolDown), timeLeft(0){}
        void step(){
            if(decreaseTimeLeft){
                completedOrderId = activeOrderId;
                activeOrderId = NO_ORDER;
            }
        }
        int getCoolDown() const{return coolDown;};
        int getTimeLeft() const{return timeLeft;};
        bool decreaseTimeLeft(){
            if(timeLeft<=0) throw std::runtime_error("already finished work");
            timeLeft--;
            return timeLeft==0;
        }
        bool hasOrdersLeft() const override{ return true; }
        bool canTakeOrder(const Order &order) const override{ return activeOrderId==NO_ORDER; }
        void acceptOrder(const Order &order) override{
            if(activeOrderId!=NO_ORDER) throw std::runtime_error("already finished work");
            activeOrderId = order.getId();
            timeLeft = coolDown;
        }
        string toString() const override{
            std::string res = Volunteer::toString();
            if(isBusy()){
                res += "\n timeLeft: " + std::to_string(getTimeLeft());
            }
            return res;
        }
    private:
        const int coolDown; // The time it takes the volunteer to process an order
        int timeLeft; // Time left until the volunteer finishes his current order
};

class LimitedCollectorVolunteer: public CollectorVolunteer {

    public:
        LimitedCollectorVolunteer(int id, string name, int coolDown ,int maxOrders);
        LimitedCollectorVolunteer *clone() const override;
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override;
        void acceptOrder(const Order &order) override;

        int getMaxOrders() const;
        int getNumOrdersLeft() const;
        string toString() const override;

        bool hasOrdersLeft() const override{ ordersLeft > 0; }
        bool canTakeOrder(const Order &order) const override{ return activeOrderId==NO_ORDER & hasOrdersLeft(); }
    
    private:
        const int maxOrders; // The number of orders the volunteer can process in the whole simulation
        int ordersLeft; // The number of orders the volunteer can still take
};

class DriverVolunteer: public Volunteer {

    public:
        DriverVolunteer(int id, string name, int maxDistance, int distancePerStep);
        DriverVolunteer *clone() const override;

        int getDistanceLeft() const;
        int getMaxDistance() const;
        int getDistancePerStep() const;  
        bool decreaseDistanceLeft(); //Decrease distanceLeft by distancePerStep,return true if distanceLeft<=0,false otherwise
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override; // Signal if the volunteer is not busy and the order is within the maxDistance
        void acceptOrder(const Order &order) override; // Reset distanceLeft to maxDistance
        void step() override; // Decrease distanceLeft by distancePerStep
        string toString() const override;

        bool hasOrdersLeft() const override{ return true; }
        bool canTakeOrder(const Order &order) const override{ return activeOrderId==NO_ORDER; }

    private:
        const int maxDistance; // The maximum distance of ANY order the volunteer can take
        const int distancePerStep; // The distance the volunteer does in one step
        int distanceLeft; // Distance left until the volunteer finishes his current order
};

class LimitedDriverVolunteer: public DriverVolunteer {

    public:
        LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders);
        LimitedDriverVolunteer *clone() const override;
        int getMaxOrders() const;
        int getNumOrdersLeft() const;
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override; // Signal if the volunteer is not busy, the order is within the maxDistance and have orders left
        void acceptOrder(const Order &order) override; // Reset distanceLeft to maxDistance and decrease ordersLeft
        string toString() const override;

        bool hasOrdersLeft() const override{ ordersLeft > 0; }
        bool canTakeOrder(const Order &order) const override{ return activeOrderId==NO_ORDER & hasOrdersLeft(); }
    private:
        const int maxOrders; // The number of orders the volunteer can process in the whole simulation
        int ordersLeft; // The number of orders the volunteer can still take
};