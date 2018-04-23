/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>

//#include <vector>

#include <yarp/gsl/impl/gsl_structs.h>

#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::sig;

template <class T>
class SenderThread:public Thread
{
public:
    SenderThread(Port *p)
    {
        portOut=p;
    }

    bool threadInit() override
    {
        success=false;
        return true;
    }

    void run() override
    {
        VectorOf<T> v;

        int times=10;

        while(times--)
        {
            v.clear();
            int k=0;
            for(k=0;k<10;k++)
                v.push_back(42);

            portOut->write(v);
            Time::delay(0.1);
            v.clear();

            for(k=0;k<42;k++)
                v.push_back(k);

            portOut->write(v);
        }

        success=true;
    }

    Port *portOut;
    bool success;
};

class Thread2b:public Thread
{
    int value;
public:
    Thread2b(Port *p)
    {
        portIn=p;
    }

    void init(int val)
    {
        value = val;
    }


    bool threadInit() override
    {
        success=false;
        return true;
    }

    void run() override
    {
        VectorOf<int> v;

        int times=10;
        bool ok=true;
        while(times--)
        {
            portIn->read(v);

            int k;
            int s=(int)v.size();
            if (s!=10)
                ok=false;
            for(k=0;k<s;k++)
            {
                if (v[k]!=42)
                    ok=false;
            }

            portIn->read(v);
            s=(int)v.size();
            if (s!=42)
                ok=false;
            for(k=0;k<s;k++)
            {
                if (v[k]!=k)
                    ok=false;
            }

        }

        success=ok;
    }

    Port *portIn;
    bool success;
};

class Thread3b :public Thread
{
public:
    Thread3b(Port *p)
    {
        portIn = p;
    }

    bool threadInit() override
    {
        success = false;
        return true;
    }

    void run() override
    {
        Bottle v;

        int times = 10;
        bool ok = true;
        while (times--)
        {
            portIn->read(v);

            int k;
            int s = (int)v.size();
            if (s != 10)
                ok = false;
            for (k = 0; k<s; k++)
            {
                if (v.get(k).asInt32() != 42)
                {

                    ok = false;
                }
            }

            portIn->read(v);

            s = (int)v.size();
            if (s != 42)
                ok = false;
            for (k = 0; k < s; k++)
            {
                if (v.get(k).asInt32() != k)
                {

                    ok = false;
                }
            }

        }

        success = ok;
    }

    Port *portIn;
    bool success;
};

class VectorOfTest : public UnitTest {

public:
    virtual std::string getName() const override { return "VectorOfTest"; }
    void checkSendReceiveInt()
    {
        report(0, "check VectorO<int> send receive");

        Port portIn;
        Port portOut;

        portOut.open("/harness_sig/vtest/o");
        portIn.open("/harness_sig/vtest/i");

        Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

        portOut.enableBackgroundWrite(true);


        VectorOf<int> vector;
        vector.resize(10);
        for (unsigned int k = 0; k < vector.size(); k++)
        {
            vector[k] = k;
        }

        portOut.write(vector);

        VectorOf<int> tmp;
        portIn.read(tmp);

        //compare vector and tmp
        bool success = true;
        if (tmp.size() != vector.size())
        {
            success = false;
        }
        else
        {
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                if (tmp[k] != vector[k])
                    success = false;
            }
        }

        checkTrue(success, "VectorOf<int> was sent and received correctly");

        report(0, "check VectorO<int> bottle compatibility");
        //write the same vector again and receive it as a bottle
        portOut.write(vector);
        Bottle tmp2;
        portIn.read(tmp2);

        //compare vector and tmp
        success = true;
        if (tmp2.size() != vector.size())
        {
            success = false;
        }
        else
        {
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                if (tmp2.get(k).asInt32() != vector[k])
                    success = false;
            }
        }

        checkTrue(success, "VectorOf<int> was received correctly in a Bottle");
    }

    void testToString()
    {
        {
            report(0, "testing toString int");
            bool ok = true;
            VectorOf<int> vec;
            std::string strToCheck = "0 1 2 3 4 5 6 7 8 9";
            for (size_t i=0; i<10; i++)
            {
                vec.push_back(i);
            }

            ok = vec.toString() == strToCheck;
            checkTrue(ok, "string correctly formatted");
        }

        {
            report(0, "testing toString double");
            bool ok = true;
            VectorOf<double> vec;
            std::string strToCheck = " 0.000000\t 1.000000\t 2.000000\t 3.000000\t 4.000000\t"
                                     " 5.000000\t 6.000000\t 7.000000\t 8.000000\t 9.000000";
            for (size_t i=0; i<10; i++)
            {
                vec.push_back(i);
            }

            ok = vec.toString() == strToCheck;
            checkTrue(ok, "string correctly formatted");
        }

    }



    virtual void runTests() override {
        Network::setLocalMode(true);
        checkSendReceiveInt();
        testToString();
        Network::setLocalMode(false);
    }
};

static VectorOfTest theVectorOfTest;

UnitTest& getVectorOfTest() {
    return theVectorOfTest;
}
