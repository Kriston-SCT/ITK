/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrapInstanceTable.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#include "wrapInstances.h"
#include "wrapException.h"

namespace _wrap_
{


/**
 * Constructor takes interpreter to which the instance table will be
 * attached.  It also initializes the temporary object number to zero.
 */
Instances::Instances(Tcl_Interp* interp):
  m_Interpreter(interp),
  m_TempNameNumber(0)
{
}


/**
 * Set a mapping from "name" to object "object" with type "type".
 * This deletes any instance of an object already having the given name.
 */
void Instances::SetObject(const String& name, void* object,
                          const CvQualifiedType& type)
{
  if(this->Exists(name))
    {
    this->DeleteObject(name);
    }
  m_InstanceMap[name] = Reference(object, type);
  m_AddressToNameMap[object] = name;  
}
  
  
/**
 * Delete the object corresponding to the given name.
 * This looks up the function pointer that was registered for the object's
 * type, and calls it to do the actual deletion.
 */
void Instances::DeleteObject(const String& name)
{
  this->CheckExists(name);
  
  const Type* type = m_InstanceMap[name].GetCvQualifiedType().GetType();
  void* object = m_InstanceMap[name].GetObject();

  // Make sure we know how to delete this object.
  if(m_DeleteFunctionMap.count(type) < 1)
    {
    // throw _wrap_UndefinedObjectTypeException(type->GetName());
    throw _wrap_UndefinedObjectTypeException("");
    }
  
  // Remove the object's address from our table.
  m_AddressToNameMap.erase(object);

  // Call the registered delete function.
  m_DeleteFunctionMap[type](object);
  
  // Remove from the instance table.
  m_InstanceMap.erase(name);
 
  // Remove the Tcl command for this instance.
  Tcl_DeleteCommand(m_Interpreter, const_cast<char*>(name.c_str()));
}
  
  
/**
 * Check if there is an object with the given name.
 */
bool Instances::Exists(const String& name) const
{
  return (m_InstanceMap.count(name) > 0);
}
  
  
/**
 * Get a pointer to the object with the given name.
 */
void* Instances::GetObject(const String& name)
{
  this->CheckExists(name);
  return m_InstanceMap[name].GetObject();
}

  
/**
 * Get the type string for the object with the given name.
 */
const CvQualifiedType& Instances::GetType(const String& name)
{
  this->CheckExists(name);
  return m_InstanceMap[name].GetCvQualifiedType();
}

  
/**
 * Allow object type deletion functions to be added.
 */
void Instances::RegisterDeleteFunction(const Type* type,
                                       DeleteFunction func)
{
  m_DeleteFunctionMap[type] = func;
}


/**
 * Create a unique name for a temporary object, and set the given object
 * to have this name.  The name chosen is returned.
 */
String Instances::CreateTemporary(void* object, const CvQualifiedType& type)
{
  char tempName[15];
  sprintf(tempName, "__temp%x", m_TempNameNumber++);
  String name = tempName;
  this->SetObject(name, object, type);
  return name;
}


/**
 * If the given object name was generated by InsertTemporary(), the object
 * is deleted.
 */
void Instances::DeleteIfTemporary(const String& name)
{
  this->CheckExists(name);
  if(name.substr(0,6) == "__temp")
    {
    this->DeleteObject(name);
    }
}


/**
 * When an instance deletes itself, this callback is made to remove it from
 * the instance table.
 */
void Instances::DeleteCallBack(void* object)
{
  if(m_AddressToNameMap.count(object) > 0)
    {
    String name = m_AddressToNameMap[object];
    this->DeleteObject(name);
    }
}


/**
 * Make sure an object with the given name exists.
 * Throw an exception if it doesn't exist.
 */
void Instances::CheckExists(const String& name) const
{
  if(!this->Exists(name))
    {
    throw _wrap_UndefinedInstanceNameException(name);
    }
}
  

} // namespace _wrap_
