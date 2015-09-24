#include "pch.h"
#include <nvutil/subject.h>
#include <nvutil/reflection.h>

namespace nvutil 
{
	void PropertyList::destroy () {}
	PropertyListImpl::PropertyListImpl (bool b) { }
	unsigned int PropertyListImpl::getPropertyCount() const { return 0; };
	 PropertyListImpl::~PropertyListImpl() {}
	PropertyId PropertyListImpl::getProperty(unsigned int index) const {return 0;}
	PropertyId PropertyListImpl::getProperty(const std::string &name) const { return 0;}
	std::string PropertyListImpl::getPropertyName(unsigned int index) const { return std::string ("");}
	std::string PropertyListImpl::getPropertyName(const PropertyId propertyId) const { return std::string ("");}
	 bool PropertyListImpl::hasProperty(const PropertyId propertyId) const {return false;}
	 bool PropertyListImpl::hasProperty(const std::string &name) const {return false;}
	  PropertyList *PropertyListImpl::clone() const{ return (new PropertyListImpl()); }
	 bool PropertyListImpl::isStatic() const { return false;}


	PropertyListChain::PropertyListChain (bool b) { }
	 PropertyListChain::~PropertyListChain() {}
	//void PropertyList::addPropertyList(PropertyList *propertyList) {}
    
	void PropertyListChain::removePropertyList(PropertyList *propertyList) {}
	unsigned int PropertyList::getPropertyCount() const { return 0;}
	PropertyId PropertyListChain::getProperty(unsigned int index) const {return 0;}
	PropertyId PropertyListChain::getProperty(const std::string &name) const { return 0;}
	std::string PropertyListChain::getPropertyName(unsigned int index) const { return std::string ("");}
	std::string PropertyListChain::getPropertyName(const PropertyId propertyId) const { return std::string ("");}
	 bool PropertyListChain::hasProperty(const PropertyId propertyId) const {return false;}
	 bool PropertyListChain::hasProperty(const std::string &name) const {return false;}
	 unsigned int PropertyListChain::getPropertyCount() const { return 0; };

	 PropertyList *PropertyListChain::clone() const{ return (new PropertyListImpl()); }
	 bool PropertyListChain::isStatic() const { return false;}
}