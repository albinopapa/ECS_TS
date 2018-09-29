#pragma once

// Observer class to inherit from user observers
template<typename ObserverTypes>
class observer : public ObserverTypes
{
public:
	// BASE CLASSES MUST AT LEAST IMPLEMENT THIS FUNCTION
	using ObserverTypes::on_notify;
};

// Template deduction guide for observer class
template<typename ObserverTypes>
observer( ObserverTypes )->observer<ObserverTypes>;


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

