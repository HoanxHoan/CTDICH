/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  // TODO
  if (type == NULL) return NULL;

  Type* newType = malloc(sizeof(Type));
  newType->typeClass = type->typeClass;

  if (type->typeClass == TP_ARRAY) {
    newType->arraySize = type->arraySize;
    newType->elementType = duplicateType(type->elementType);
  }
  return newType;
}

int compareType(Type* type1, Type* type2) {
  // TODO
  if(type1->typeClass == type2->typeClass) {
    if(type1->typeClass == TP_ARRAY) { 
      if(type1->arraySize != type2->arraySize || compareType(type1->elementType, type2->elementType) == 0) {
        return 0;
      } else {
        return 1; 
      }
    }
  }

  return 0; // type1->typeClass != type2->typeClass
}

void freeType(Type* type) {
  // TODO
  if (type == NULL) return;
  if (type->typeClass == TP_ARRAY)
      freeType(type->elementType);
  free(type);
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  // TODO
  ConstantValue *newIntConstantValue = (ConstantValue*)malloc(sizeof(ConstantValue));
  newIntConstantValue->type = TP_INT;
  newIntConstantValue->intValue = i;
  return newIntConstantValue;
}

ConstantValue* makeCharConstant(char ch) {
  // TODO
  ConstantValue *newCharConstantValue = (ConstantValue*)malloc(sizeof(ConstantValue));
  newCharConstantValue->type = TP_CHAR;
  newCharConstantValue->charValue = ch;
  return newCharConstantValue;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // TODO
  ConstantValue *newConstantValue = (ConstantValue*)malloc(sizeof(ConstantValue));
  newConstantValue->type = v->type;
  if(v->type == TP_CHAR) {
    newConstantValue->type = TP_CHAR;
  } else if(v->type == TP_INT) {
    newConstantValue->type = TP_INT;
  }
  return newConstantValue;
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {
  // TODO
  Object* constant = (Object*) malloc(sizeof(Object));
  strcpy(constant->name, name);
  constant->kind = OBJ_CONSTANT;
  constant->constAttrs = (ConstantAttributes*) malloc(sizeof(ConstantAttributes));
  constant->constAttrs->value = (ConstantValue*) malloc(sizeof(ConstantValue));
  return constant;
}

Object* createTypeObject(char *name) {
  // TODO
  Object* type = (Object*) malloc(sizeof(Object));
  strcpy(type->name, name);
  type->kind = OBJ_TYPE;  
  type->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  type->typeAttrs->actualType = (Type*) malloc(sizeof(Type));
  return type;
}

Object* createVariableObject(char *name) {
  // TODO
  Object* variable = (Object*) malloc(sizeof(Object));
  strcpy(variable->name, name);
  variable->kind = OBJ_VARIABLE;
  variable->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  variable->varAttrs->scope = createScope(variable, symtab->currentScope);
  variable->varAttrs->type = (Type*) malloc(sizeof(Type));
  return variable;
}

Object* createFunctionObject(char *name) {
  // TODO
  Object* function = (Object*) malloc(sizeof(Object));
  strcpy(function->name, name);
  function->kind = OBJ_FUNCTION;
  function->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  function->funcAttrs->paramList = NULL;
  function->funcAttrs->scope = createScope(function, symtab->currentScope);
  function->funcAttrs->returnType = (Type*) malloc(sizeof(Type));
  return function;
}

Object* createProcedureObject(char *name) {
  // TODO
  Object* procedure = (Object*) malloc(sizeof(Object));
  strcpy(procedure->name, name);
  procedure->kind = OBJ_PROCEDURE;
  procedure->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  procedure->procAttrs->paramList = NULL;
  procedure->procAttrs->scope = createScope(procedure, symtab->currentScope);
  return procedure;
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // TODO
  Object* parameter = (Object*) malloc(sizeof(Object));
  strcpy(parameter->name, name);
  parameter->kind = OBJ_PARAMETER;
  parameter->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  parameter->paramAttrs->kind = kind;
  parameter->paramAttrs->function = owner;
  parameter->paramAttrs->type = (Type*) malloc(sizeof(Type));
  return parameter;
}

void freeObject(Object* obj) {
  // TODO
  if(obj != NULL){
    switch(obj->kind){
      case OBJ_CONSTANT:
      if(obj->constAttrs->value != NULL){
        free(obj->constAttrs->value);
      }
        free(obj->constAttrs);
        break;
      case OBJ_VARIABLE:
      if(obj->varAttrs->type != NULL){
        freeType(obj->varAttrs->type);
      }
      if(obj->varAttrs->scope != NULL){
        freeScope(obj->varAttrs->scope);
      }
        free(obj->varAttrs);
        obj->varAttrs = NULL;
        break;
      case OBJ_TYPE:
      if(obj->typeAttrs->actualType != NULL){
        freeType(obj->typeAttrs->actualType);
      }
        free(obj->typeAttrs);
        break;
      case OBJ_FUNCTION:
      if(obj->funcAttrs->returnType != NULL){
        freeType(obj->funcAttrs->returnType);
      }
      if(obj->funcAttrs->scope != NULL){
        freeScope(obj->funcAttrs->scope);
      }
      if(obj->funcAttrs->paramList != NULL){
        freeObjectList(obj->funcAttrs->paramList);
      }
        free(obj->funcAttrs);
        break;
      case OBJ_PROCEDURE:
      if(obj->procAttrs->scope != NULL){
        freeScope(obj->procAttrs->scope);
      }
      if(obj->procAttrs->paramList != NULL){
        freeObjectList(obj->procAttrs->paramList);
      }
        free(obj->procAttrs);
        break;
      case OBJ_PARAMETER:
      if(obj->paramAttrs->type != NULL){
        freeType(obj->paramAttrs->type);
      }
        free(obj->paramAttrs);
        break;
      case OBJ_PROGRAM:
        freeScope(obj->progAttrs->scope);
        free(obj->progAttrs);
        break;
      default:
        break;
    }
    free(obj);
  }
}

void freeScope(Scope* scope) {
  // TODO
  if(scope != NULL){
    freeObjectList(scope->objList);
    free(scope);
    scope = NULL;
  }
}

void freeObjectList(ObjectNode *objList) {
  // TODO
  if(objList !=NULL){
    freeObjectList(objList->next);      
    freeObject(objList->object);       
    free(objList); 
  }
}

void freeReferenceList(ObjectNode *objList) {
  // TODO
  ObjectNode* temp = objList;
  while(temp != NULL){
    ObjectNode* next = temp->next;
    free(temp);
    temp = next;
  }
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // TODO
  ObjectNode* current = objList;
  while(current != NULL){
    if(strcmp(current->object->name, name) == 0){
      return current->object; 
    }
  }  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  addObject(&(symtab->currentScope->objList), obj);
}


