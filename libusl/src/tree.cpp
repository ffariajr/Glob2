#include "tree.h"
#include "code.h"
#include "debug.h"
#include "error.h"
#include <sstream>

using namespace std;


void Node::generate(ThunkPrototype* thunk, DebugInfo* debug, Code* code)
{
	thunk->body.push_back(code);
	
	if (debug != 0)
	{
		size_t address = thunk->body.size();
		ThunkDebugInfo* scopeDebug = debug->get(thunk);
		ThunkDebugInfo::Source2Address::iterator it = scopeDebug->source2Address.find(position);
		if (it != scopeDebug->source2Address.end())
		{
			scopeDebug->address2Source.erase(it->second);
			it->second = address;
		}
		else
		{
			scopeDebug->source2Address[position] = address;
		}
		scopeDebug->address2Source[address] = position;
	}
}

void Node::dump(std::ostream &stream, unsigned indent) const
{
	for (unsigned i = 0; i < indent; ++i)
		stream << '\t';
	stream << position.line << ":" << position.column << ": " << unmangle(typeid(*this).name());
	dumpSpecific(stream, indent);
}

void Node::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << '\n';
}


void ExpressionNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	generate(static_cast<ThunkPrototype*>(scope), debug, heap);
}


void ConstNode::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	Node::generate(thunk, debug, new ConstCode(value));
}

void ConstNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << ' ';
	value->dump(stream);
	stream << '\n';
}


SelectNode::~SelectNode()
{
	delete receiver;
}

void SelectNode::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	receiver->generate(thunk, debug, heap);
	Node::generate(thunk, debug, new SelectCode(name));
	Node::generate(thunk, debug, new EvalCode());
}

void SelectNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << "(" << name << ")";
	stream << '\n';
	receiver->dump(stream, indent + 1);
}


ApplyNode::~ApplyNode()
{
	delete function;
	delete argument;
}

void ApplyNode::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	ThunkPrototype* arg = new ThunkPrototype(heap, thunk);
	argument->generate(arg, debug, heap);
	Node::generate(arg, debug, new ReturnCode());
	
	function->generate(thunk, debug, heap);
	Node::generate(thunk, debug, new ThunkCode());
	Node::generate(thunk, debug, new CreateCode<Thunk>(arg));
	Node::generate(thunk, debug, new ApplyCode());
}

void ApplyNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << '\n';
	function->dump(stream, indent + 1);
	argument->dump(stream, indent + 1);
}


DecNode::~DecNode()
{
	delete body;
}

void DecNode::declare(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	scope->members[name] = new ThunkPrototype(heap, scope);
	
	if (type == VAR)
	{
		// TODO: setter
	}
}

void DecNode::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	ScopePrototype* scope = dynamic_cast<ScopePrototype*>(thunk);
	assert(scope);
	generate(scope, debug, heap);
}

void DecNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	switch (type) {
	case AUTO:
	case DEF:
		{
			ThunkPrototype* def = scope->members[name];
			assert(def);
			body->generate(def, debug, heap);
			Node::generate(def, debug, new ReturnCode());

			Node::generate(scope, debug, new ConstCode(&nil));
		}
		break;
	case VAR:
	case VAL:
		{
			size_t index = scope->locals.size();
			scope->locals.push_back(name);
			
			body->generate(scope, debug, heap);
			Node::generate(scope, debug, new DupCode());
			Node::generate(scope, debug, new ValCode(index));
			
			ThunkPrototype* getter = scope->members[name];
			Node::generate(getter, debug, new ThunkCode());
			Node::generate(getter, debug, new ParentCode());
			Node::generate(getter, debug, new ValRefCode(index));
			Node::generate(getter, debug, new ReturnCode());
		}
		break;
	}
}

void DecNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << "(" << name << "," << type << ")";
	stream << '\n';
	body->dump(stream, indent + 1);
}


BlockNode::~BlockNode()
{
	for (Elements::iterator it = elements.begin(); it != elements.end(); ++it)
	{
		delete *it;
	}
}

void BlockNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << '\n';
	for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it)
	{
		(*it)->dump(stream, indent + 1);
	}
}


void ExecutionBlock::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	if (!elements.empty())
	{
		ScopePrototype* block = new ScopePrototype(heap, thunk);
		block->members["this"] = thisMember(block);

		for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it)
		{
			DecNode* dec = dynamic_cast<DecNode*>(*it);
			if (dec != 0)
				dec->declare(block, debug, heap);
		}
	
		for (Elements::const_iterator it = elements.begin(); it != elements.end() - 1; ++it)
		{
			Node* element = *it;
			element->generate(block, debug, heap);
			Node::generate(block, debug, new PopCode());
		}
	
		elements.back()->generate(block, debug, heap);
		Node::generate(block, debug, new ReturnCode());
	
		Node::generate(thunk, debug, new ThunkCode());
		Node::generate(thunk, debug, new CreateCode<Scope>(block));
		Node::generate(thunk, debug, new EvalCode());
	}
	else
	{
		Node::generate(thunk, debug, new ConstCode(&nil));
	}
}


void RecordBlock::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	ScopePrototype* block = new ScopePrototype(heap, thunk);
	block->members["this"] = thisMember(block);
	//block->members["get"] = getMember(block);

	for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it)
	{
		DecNode* dec = dynamic_cast<DecNode*>(*it);
		if (dec)
			dec->declare(block, debug, heap);
	}

	for (Elements::const_iterator it = elements.begin(); it != elements.end(); ++it)
	{
		size_t index = block->locals.size();

		Node* element = *it;
		element->generate(block, debug, heap);

		ThunkPrototype* getter;

		DecNode* dec = dynamic_cast<DecNode*>(element);
		if (dec)
		{
			getter = block->members[dec->name];
		}
		else
		{
			block->locals.push_back("");
			Node::generate(block, debug, new ValCode(index));

			getter = new ThunkPrototype(heap, block);
			Node::generate(getter, debug, new ThunkCode());
			Node::generate(getter, debug, new ParentCode());
			Node::generate(getter, debug, new ValRefCode(index));
			Node::generate(getter, debug, new ReturnCode());
		}

		stringstream str;
		str << index;
		block->members[str.str()] = getter;
	}

	Node::generate(block, debug, new ThunkCode());
	Node::generate(block, debug, new ReturnCode());

	Node::generate(thunk, debug, new ThunkCode());
	Node::generate(thunk, debug, new CreateCode<Scope>(block));
	Node::generate(thunk, debug, new EvalCode());
}


void DefLookupNode::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	Node::generate(thunk, debug, new ThunkCode());
	
	ThunkPrototype* member;
	Prototype* prototype = thunk;
	while (true)
	{
		member = prototype->lookup(name);
		if (member != 0)
			break;
		
		ThunkPrototype* t = dynamic_cast<ThunkPrototype*>(prototype);
		assert(t != 0);
		
		Node::generate(thunk, debug, new ParentCode());
		
		prototype = t->outer;
		
		if (prototype == 0)
		{
			ostringstream message;
			message << "Program error @" << position << ":" << endl;
			message << "Declaration not found: " << name << endl;
			throw Exception(position, message.str());
		}
	}
	
	Node::generate(thunk, debug, new CreateCode<Thunk>(member));
	Node::generate(thunk, debug, new EvalCode());
}

void DefLookupNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << "(" << name << ")";
	stream << '\n';
}


void IgnorePatternNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	Node::generate(scope, debug, new PopCode());
}


void NilPatternNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	// TODO: check we really got nil
	Node::generate(scope, debug, new PopCode());
}


void ValPatternNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	size_t index = scope->locals.size();
	scope->locals.push_back(name);
	
	Node::generate(scope, debug, new EvalCode());
	Node::generate(scope, debug, new ValCode(index));
	
	ScopePrototype* getter = new ScopePrototype(heap, scope);
	scope->members[name] = getter;
	
	Node::generate(getter, debug, new ThunkCode());
	Node::generate(getter, debug, new ParentCode());
	Node::generate(getter, debug, new ValRefCode(index));
	Node::generate(getter, debug, new ReturnCode());
}

void ValPatternNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << "(" << name << ")";
	stream << '\n';
}


void DefPatternNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	size_t index = scope->members.size();
	scope->locals.push_back(name);
	
	Node::generate(scope, debug, new ValCode(index));
	
	ScopePrototype* getter = new ScopePrototype(heap, scope);
	scope->members[name] = getter;
	
	Node::generate(getter, debug, new ThunkCode());
	Node::generate(getter, debug, new ParentCode());
	Node::generate(getter, debug, new ValRefCode(index));
	Node::generate(getter, debug, new EvalCode());
	Node::generate(getter, debug, new ReturnCode());
}

void DefPatternNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << "(" << name << ")";
	stream << '\n';
}


TuplePatternNode::~TuplePatternNode()
{
	for (Members::iterator it = members.begin(); it != members.end(); ++it)
	{
		delete *it;
	}
}

void TuplePatternNode::generate(ScopePrototype* scope, DebugInfo* debug, Heap* heap)
{
	Node::generate(scope, debug, new EvalCode());
	int index = 0;
	for (Members::iterator it = members.begin(); it != members.end(); ++it)
	{
		stringstream str;
		str << index;

		Node::generate(scope, debug, new DupCode());
		Node::generate(scope, debug, new SelectCode(str.str()));
		(*it)->generate(scope, debug, heap);
		++index;
	}
	Node::generate(scope, debug, new PopCode());
}

void TuplePatternNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << '\n';
	for (Members::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		(*it)->dump(stream, indent + 1);
	}
}


FunNode::~FunNode()
{
	delete arg;
	delete body;
}

void FunNode::generate(ThunkPrototype* thunk, DebugInfo* debug, Heap* heap)
{
	Method* method = new Method(heap, thunk);
	arg->generate(method, debug, heap);
	body->generate(method, debug, heap);
	Node::generate(method, debug, new ReturnCode());
	
	Node::generate(thunk, debug, new ThunkCode());
	Node::generate(thunk, debug, new CreateCode<Function>(method));
}

void FunNode::dumpSpecific(std::ostream &stream, unsigned indent) const
{
	stream << '\n';
	arg->dump(stream, indent + 1);
	body->dump(stream, indent + 1);
}
