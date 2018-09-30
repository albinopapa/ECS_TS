#pragma once
#include <variant>
#include <vector>

class observer_base
{
public:
	virtual void on_notify() = 0;
};
// Observer class to inherit from user observers
template<typename ObserverType>
class observer : public ObserverType 
{
public:
	// BASE CLASSES MUST AT LEAST IMPLEMENT THIS FUNCTION
	using ObserverType::on_notify;
};

// Template deduction guide for observer class
template<typename ObserverType>
observer( ObserverType )->observer<ObserverType>;


// Subject class to inherit from user subjects
template<typename...SubjectType>
class subject : public SubjectType...
{
public:
	// BASE CLASSES MUST AT LEAST IMPLEMENT THESE FUNCTIONS
	using SubjectType::add_observer;
	using SubjectType::remove_observer;
	using SubjectType::notify;
};

template<typename...SubjectType>
subject( SubjectType... )->subject<SubjectType...>;
// END SUBJECT
