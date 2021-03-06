#include <iostream>
#include <memory>

#include <simpleFsm/fsm.h>
#include <simpleFsm/bindablestate.h>
#include <simpleFsm/terminationstate.h>

using namespace std;
using namespace simpleFsm;

class InitFsm : public simpleFsm::Fsm
{
public:
    InitFsm(IFsm& parent, int& foo, int& bar) : Fsm(parent, "InitFsm") {

        auto initFoo = make_shared<BindableState>(*this, [&] {cout << "InitFoo::Enter()" << endl; foo = 0;});
        auto initBar = make_shared<BindableState>(*this, [&] {cout << "InitBar::Enter()" << endl; bar = 0;});

        initFoo->AddTransition(make_shared<Transition>(*this, initBar));
        initBar->AddTransition(make_shared<Transition>(*this, m_endState));

        SetFirstState(initFoo);
    }
};

class CountFsm : public simpleFsm::Fsm
{
public:
    CountFsm(IFsm& parent, int& foo, int& bar) : Fsm(parent, "CountFsm") {
        auto countFoo = make_shared<BindableState>(*this, [&] {cout << "CountFoo::Enter()" << endl; foo++;});
        auto countBar = make_shared<BindableState>(*this, [&] {cout << "CountBar::Enter()" << endl; bar--;});

        countFoo->AddTransition(make_shared<Transition>(*this, countBar));
        countBar->AddTransition(make_shared<Transition>(*this, m_endState));

        SetFirstState(countFoo);
    }
};


class DisplayFsm : public simpleFsm::Fsm
{
public:
    DisplayFsm(IFsm& parent, int& foo, int& bar) : Fsm(parent, "DisplayFsm") {

        auto displayFoo = make_shared<BindableState>(
                    *this,
                    [&] {cout << "DisplayFoo::Enter()" << endl;
                         cout << "foo = " << foo << endl; });

        auto displayBar = make_shared<BindableState>(
                    *this,
                    [&] {cout << "DisplayBar::Enter()" << endl;
                         cout << "bar = " << bar << endl; });


        displayFoo->AddTransition(make_shared<Transition>(*this, displayBar));
        displayBar->AddTransition(make_shared<Transition>(*this, m_endState));
        SetFirstState(displayFoo);

    }
};

class ExampleFsm : public simpleFsm::Fsm
{
public:
    ExampleFsm() : Fsm("ExampleFsm") {

        auto init = make_shared<InitFsm>(*this, m_foo, m_bar);
        auto count = make_shared<CountFsm>(*this, m_foo, m_bar);
        auto display = make_shared<DisplayFsm>(*this, m_foo, m_bar);

        init->AddTransition(make_shared<Transition>(*this, count));
        count->AddTransition(make_shared<Transition>(*this, display));

        std::function<bool()> exitGaurd = [&] {
            return ((m_foo > 2) && (m_bar < 2));
        };
        display->AddTransition(make_shared<Transition>(*this, make_shared<TerminationState>(*this), exitGaurd));
        display->AddTransition(make_shared<Transition>(*this, count));

        SetFirstState(init);

    }

protected:
    int m_foo;
    int m_bar;
};


int main() {
    ExampleFsm fsm;

    for(;;) {
        std::cout << "---" << std::endl;
        if(!fsm.Step()) break;
    }

}

