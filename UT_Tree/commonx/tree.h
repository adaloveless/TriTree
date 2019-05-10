#ifndef _TREE_H_
#define _TREE_H_
#include "sharedobject.h"
#ifdef TREE_DEBUG
#include <iostream>
#endif


//********************************************************************
//This is the ITEM class.  A tree contains a sorted list of THESE.
//The TREE class is below
//IMPORTANT NOTE: Be sure to OVERRIDE the abstract Compare() function
class TriTreeItem : public BetterObject //TBTreeItem = class abstract(TBetterObject)
{
public:
	friend class TriTree;
protected:
	
	//RELATED NODES
	//I wish these were private, but 
	TriTreeItem *m_LeftNode, *m_RightNode, *m_UpperNode, *m_LowerNode;
public:


	//		
	void AddDuplicate(TriTreeItem* itm)
	{
		TriTreeItem* oldlower = m_LowerNode;
		m_LowerNode = itm;
		itm->m_UpperNode = this;

		if (oldlower != NULL)
		{
			oldlower->m_UpperNode = itm;
			itm->m_LowerNode = oldlower;
		}
	}

	
	virtual ni Compare(TriTreeItem* CompareTo) ABSTRACT_FUNCTION;	
	void* m_Tree;//[unsafe] Ftree: TObject;
	ni m_Balance;		//FBalance : -2 .. 2;
	TriTreeItem(void)
		: m_UpperNode(NULL), m_LowerNode(NULL), m_RightNode(NULL),
		m_Balance(NULL), m_Tree(NULL), m_LeftNode(NULL)
	{}

	~TriTreeItem(void) { };//destructor Destroy; override;
	void Isolate(void)
	{
		m_LowerNode = NULL;
		m_UpperNode = NULL;
		m_LeftNode = NULL;
		m_RightNode = NULL;
		m_Balance = 0;
		m_Tree = NULL;
	}

	TriTreeItem* LastChild(void) {
		if (m_RightNode == NULL)
			return this;
		else
			return m_RightNode->LastChild();
	}

	ni GetHeight(void)
	{
		ni l = -1, r = -1;
		if (m_LeftNode != NULL)
		{
			l = m_LeftNode->GetHeight();
		}

		if (m_RightNode != NULL)
		{
			r = m_RightNode->GetHeight();
		}

		return GREATER_OF(l, r) + 1;
	}
	inline bool NeedsRebalance(void) { return ABS(m_Balance) > 1; };
	void* getTree(void) { return m_Tree; };
	void setTree(void* tree) { m_Tree = tree; }

/*	
	Experimental
	PTriTree Tree = PROPERTY<PTriTree>(
		[&]() -> PTriTree& { return m_Tree; },
		[&](PTriTree newValue) { m_Tree = newValue; }
	);
	*/
};
//property tree: TObject read FTree write SetTree;
//end;

typedef TriTreeItem* PTriTreeItem;



//*****************************************************
//THIS IS THE TREE class which will contain a list of TriTreeItem descendant instances
// Add() Adds an item
// Remove() to remove an item
// Iterate() to walk through the list in sort-order.
class TriTree : public SharedObject
{
	friend class TriTreeItem;
public:
	TriTree() : m_Root(NULL), m_Count(0) {  }
	~TriTree() { ClearBruteForce(); }

private:
	ni m_Count;
	PTriTreeItem m_Root;

protected:
	
	//*********************************************
	//THIS struct is used by the delete operation to manage
	//the most-complex "rotation" of the tree
	//There is a rare scenario in which a leaf node needs to be transplanted
	//as high as the root and maintaining "pointer perfect" operation is
	// a bit messy.
	typedef struct
	{

		PTriTreeItem delete_me, replace_me;
		PTriTreeItem restack_bottom, restack_top;
		PTriTreeItem tmp, tmp2;
	} delete_operation_state_t;

	void PrivateDel2(delete_operation_state_t* state, PTriTreeItem& r, bool& balance_changed)
	{
		if (r->m_RightNode != NULL)
		{
			PrivateDel2(state, r->m_RightNode, balance_changed);
			if (balance_changed)
				DecBalance(r, balance_changed, true);
		}
		else
		{
			state->tmp = r;
			state->tmp2 = r->m_LeftNode;
			state->tmp->m_Balance = state->replace_me->m_Balance;
			state->tmp->m_LeftNode = state->replace_me->m_LeftNode;
			state->tmp->m_RightNode = state->replace_me->m_RightNode;
			state->replace_me = state->tmp;
			state->restack_top = state->delete_me;
			state->restack_bottom = state->tmp;
			r = state->tmp2;
			balance_changed = true;
		}
	}
	void PrivateDelete(PTriTreeItem& itm, PTriTreeItem& p, bool& balance_changed, bool bDontFree)
	{
		delete_operation_state_t state;
		state.delete_me = NULL;
		state.restack_bottom = NULL;
		state.restack_top = NULL;
		if (p == NULL)
		{
			balance_changed = false;
#ifdef TRITREE_DEBUG
			if (SlowSearch(itm) >= 0)
				throw "item was found, but not properly placed in the tree";//critical error, means tree is not working
#endif
			throw "item was not found during delete";//critical error, you tried to delete something not in the tree			
		}
		else
		{
			ni compared = itm->Compare(p);

			if (compared > 0)
			{
				PrivateDelete(itm, p->m_LeftNode, balance_changed, bDontFree);
				if (p == state.restack_top) p = state.restack_bottom;
				if (balance_changed)
					IncBalance(p, balance_changed, true);

			}
			else if (compared < 0)
			{
				PrivateDelete(itm, p->m_RightNode, balance_changed, bDontFree);
				if (p == state.restack_top) p = state.restack_bottom;
				if (p != NULL)
					if (balance_changed)
						IncBalance(p, balance_changed, true);
			}
			else
			{
				state.tmp = p;
				if (state.tmp->m_LowerNode != NULL)
				{
					while (state.tmp != itm)
					{
						state.tmp = state.tmp->m_LowerNode;
						if (state.tmp == NULL)
						{
							throw "Critical! linked node not found!";
						}
					}

					state.delete_me = state.tmp;
					balance_changed = false;
					if (state.tmp->m_LowerNode != NULL)
					{
						state.tmp->m_LowerNode->m_UpperNode = state.tmp->m_LowerNode;
						state.tmp->m_LowerNode->m_LeftNode = state.tmp->m_LeftNode;
						state.tmp->m_LowerNode->m_RightNode = state.tmp->m_RightNode;
						state.tmp->m_LowerNode->m_Balance = state.tmp->m_Balance;
						if (state.tmp->m_UpperNode != NULL)
						{
							state.tmp->m_UpperNode->m_LowerNode = state.tmp->m_LowerNode;
						}
						state.tmp = state.tmp->m_LowerNode;
						if (p == state.delete_me)
							p = state.tmp;

					}
					else
					{
						if (state.tmp->m_UpperNode != NULL)
						{
							state.tmp->m_UpperNode->m_LowerNode = state.tmp->m_LowerNode;
							state.delete_me = state.tmp;
							state.tmp = NULL;
						}
					}
				}
				else
				{
					state.replace_me = p;
					state.delete_me = state.replace_me;
					if (state.delete_me->m_RightNode == NULL)
					{
						p = state.delete_me->m_LeftNode;
						balance_changed = true;
					}
					else if (state.delete_me->m_LeftNode == NULL)
					{
						p = state.delete_me->m_RightNode;
						balance_changed = true;
					}
					else
					{
						PrivateDel2(&state, state.delete_me->m_LeftNode, balance_changed);
						if (p == state.restack_top)
						{
							ni temp = state.restack_bottom->m_Balance;
							//restack_bottom's left node, was assigned by this reference variable
							//therefore, the address still points to the original deletion point
							//it needs to get reassigned as well
							state.restack_bottom->m_LeftNode = state.delete_me->m_LeftNode;
							p = state.restack_bottom;
							p->m_Balance = temp;
						}
						if (balance_changed)
							IncBalance(p, balance_changed, true);

					}
				}
			}

			if (state.delete_me == NULL)
				throw "did not find anything to delete";
			if (bDontFree == false)
			{
				delete state.delete_me;
			}
			else {
				state.delete_me->Isolate();
			}

			m_Count--;

		}
	};


	//********************************************************
	//PROTECTED
	//PLACES an ITEM into the tree (used by the ADD function)		
	void SearchAndInsert(PTriTreeItem itm, PTriTreeItem& p, bool& balance_changed, bool& found)
	{
		found = false;//Found: = false;
		if (p == NULL)
		{
			p = itm;
			balance_changed = true;
			if (this->m_Root == NULL) this->m_Root = p;
			p->m_LeftNode = NULL;
			p->m_RightNode = NULL;
			p->m_Balance = NULL;
		}
		else
		{
			ni compared = itm->Compare(p);
			if (compared > 0) {
				SearchAndInsert(itm, p->m_LeftNode, balance_changed, found);
				if ((balance_changed) && (!found))
					DecBalance(p, balance_changed, false);
			}
			else if (compared < 0)
			{
				SearchAndInsert(itm, p->m_RightNode, balance_changed, found);
				if ((balance_changed) && (!found))
					IncBalance(p, balance_changed, false);
			}
			else {
				balance_changed = false;
				p->AddDuplicate(itm);
			}
		}
	};

protected:
	typedef std::function<void(PTriTreeItem itm)> TSimpleIterateProcedure;
	//*********************************************************
	// Private Used by Iterate() function to actually walk the tree
	void SimpleListItems(PTriTreeItem TreeItem, TSimpleIterateProcedure proc)
	{
		//this is a key function
		if (TreeItem == NULL)
			return;
		//FIRST call proc with the LEFT side of the TREE
		if (TreeItem->m_LeftNode != NULL) SimpleListItems(TreeItem->m_LeftNode, proc);

		//call PROC on THIS
		proc(TreeItem);

		//call PROC on the CENTER (which is essentially a linked list)
		PTriTreeItem lower = TreeItem->m_LowerNode;
		while (lower != NULL)
		{
			proc(lower);
			lower = lower->m_LowerNode;
		}

		//call PROC on RIGHT tree
		if (TreeItem->m_RightNode != NULL) SimpleListItems(TreeItem->m_RightNode, proc);
	}

	
	typedef std::function<void(PTriTreeItem itm, bool& needstop)> TIterateProcedure;
	//*********************************************************
	// Private Used by Iterate() function to actually walk the tree		
	void ListItems(PTriTreeItem TreeItem, TIterateProcedure proc, bool& needstop)
	{
		if (TreeItem == NULL) return;
		//FIRST call proc with the LEFT side of the TREE
		if (TreeItem->m_LeftNode != NULL) ListItems(TreeItem->m_LeftNode, proc, needstop);
		if (needstop) return;
		//call PROC on THIS
		proc(TreeItem, needstop);
		if (needstop) return;

		//call PROC on the CENTER (which is essentially a linked list)
		PTriTreeItem lower = TreeItem->m_LowerNode;
		while (lower != NULL)
		{
			proc(lower, needstop);
			if (needstop) return;
			lower = lower->m_LowerNode;
		}

		//call PROC on RIGHT tree
		if (TreeItem->m_RightNode != NULL) ListItems(TreeItem->m_RightNode, proc, needstop);

	}

public:

	inline ni GetCount(void) { return m_Count; };

	bool IsEmpty() { return m_Root == NULL; };

	void ClearBruteForce()
	{
		while (m_Root != NULL) Remove(m_Root);
		m_Count = 0;
	}
	virtual bool Add(PTriTreeItem AItem)
	{
		bool h = false, found = false;
		AItem->setTree(this);
		SearchAndInsert(AItem, m_Root, h, found);
		bool res = !found;
		if (res)
			m_Count++;

		return res;
	};

	virtual bool Remove(PTriTreeItem AItem, bool bDontFree = false)
	{
		bool balance_changed = false;
		PrivateDelete(AItem, m_Root, balance_changed, bDontFree);
		return true;
	};




	//***********************************
	//ITERATE using simple interface
	void Iterate(const TSimpleIterateProcedure code)
	{
		SimpleListItems(m_Root, code);
	}

	//***********************************
	//Iterate with extra param.
	//needstop can be set to break the Iterate operation
	void Iterate(const TIterateProcedure code)
	{
		bool needstop = false;
		ListItems(m_Root, code, needstop);
	}

	//***********************************
	//* returns the highest indexed item in the tree	
	PTriTreeItem LastItem()
	{
		PTriTreeItem res = m_Root;
		if (res != NULL)
		{
			while (res->m_RightNode != NULL)
			{
				res = res->m_RightNode;
			}
		}
		return res;
	};
	
	//***********************************
	//* returns the lowest indexed item in the tree	
	PTriTreeItem FirstItem()
	{
		PTriTreeItem res = m_Root;
		if (res != NULL)
		{
			while (res->m_LeftNode != NULL)
			{
				res = res->m_LeftNode;
			}
		}
		return res;
	};

	//***********************************
	// MERGE another tree with this one
	// tree: tree to be merged into this one (will be emptied by this process)
	void AddTree(TriTree* tree)
		//merge another tree into this tree
		//removes items from tree, places in THIS tree
	{
		PTriTreeItem ti;
		do
		{
			ti = tree->m_Root;
			if (ti == NULL)
				break;
			tree->Remove(ti, true);
			this->Add(ti);
		} while (1);


	};
private:
	//**********************************************************************
	//deccrment balance of a node and 
	//SIDE-EFFECT triggers auto-rebalancing if necssary			
	//This function is called RECURSIVELY
	//		p is by reference (pointer to pointer, essentially)
	//		If p is changed it is used after the return in calling 
	//		layer to handle "rotations" required to balance the tree
	void DecBalance(PTriTreeItem& p, bool& balance_changed, bool fordelete)
	{
		PTriTreeItem p1, p2;
		switch (p->m_Balance)
		{
		case 1:
			p->m_Balance = 0;
			if (!fordelete)
				balance_changed = false;
			break;
		case 0:
			p->m_Balance = -1;
			if (fordelete)
				balance_changed = false;
			break;
		case -1:
			// new balancing
			p1 = p->m_LeftNode;
			if ((p1->m_Balance == -1) || ((p1->m_Balance == 0) && fordelete))
			{
				//single ll rotation
				p->m_LeftNode = p1->m_RightNode;
				p1->m_RightNode = p;
				if (!fordelete)
					p->m_Balance = 0;
				else
				{
					if (p1->m_Balance == 0)
					{
						p->m_Balance = -1;
						p1->m_Balance = 1;
						balance_changed = false;
					}
					else
					{
						p->m_Balance = 0;
						p1->m_Balance = 0;
						//(*h= false; *)
					}
				}
				p = p1;
			}			
			else
			{ 
				// double lr rotation
				p2 = p1->m_RightNode;
				p1->m_RightNode = p2->m_LeftNode;
				p2->m_LeftNode = p1;
				p->m_LeftNode = p2->m_RightNode;
				p2->m_RightNode = p;
				if (p2->m_Balance == -1)
					p->m_Balance = 1;
				else
					p->m_Balance = 0;
				if (p2->m_Balance == 1)
					p1->m_Balance = -1;
				else
					p1->m_Balance = 0;
				p = p2;
				if (fordelete)
					p2->m_Balance = 0;
			}
			if (!fordelete)
			{
				p->m_Balance = 0;
				balance_changed = false;
			}
			break;
		}
	}

	//**********************************************************************	
	//increment balance of a node and 
	//SIDE-EFFECT triggers auto-rebalancing if necssary			
	//This function is called RECURSIVELY
	//		p is by reference (pointer to pointer, essentially)
	//		If p is changed it is used after the return in calling 
	//		layer to handle "rotations" required to balance the tree
	void IncBalance(PTriTreeItem& p, bool& balance_changed, bool fordelete)
	{
		PTriTreeItem p1, p2;
		switch (p->m_Balance)
		{
		case -1:
			p->m_Balance = 0;
			if (!fordelete)
				balance_changed = false;
			break;
		case 0:
			p->m_Balance = 1;
			if (fordelete)
				balance_changed = false;
			break;
		case 1:
			p1 = p->m_RightNode;
			if ((p1->m_Balance == 1) || ((p1->m_Balance == 0) && fordelete))
			{
				//single rr rotate;
				p->m_RightNode = p1->m_LeftNode;
				p1->m_LeftNode = p;
				if (!fordelete)
					p->m_Balance = 0;
				else
				{
					if (p1->m_Balance == 0)
					{
						p->m_Balance = 1;
						p1->m_Balance = -1;
						balance_changed = false;
					}
					else {
						p->m_Balance = 0;
						p1->m_Balance = 0;
						/*h: = false; */
					}
				}
				p = p1;
			}
			else {
				// double rl rotation
				p2 = p1->m_LeftNode;
				p1->m_LeftNode = p2->m_RightNode;
				p2->m_RightNode = p1;
				p->m_RightNode = p2->m_LeftNode;
				p2->m_LeftNode = p;
				if (p2->m_Balance == 1)
					p->m_Balance = -1;
				else
					p->m_Balance = 0;

				if (p2->m_Balance == -1)
					p1->m_Balance = 1;
				else
					p1->m_Balance = 0;
				p = p2;
				if (fordelete)
					p2->m_Balance = 0;
			}

			if (!fordelete)
			{
				p->m_Balance = 0;
				balance_changed = false;
			}
			break;
		}
	};


#ifdef TREE_DEBUG
	//************************************************
	//Debug function (because this thing didn't always WORK)
	//also exposes dirty secrects
	void DebugTree(void)
	{
		TriTree* tt = this;
		system("CLS");
		tt->Iterate(
			[=](PTriTreeItem itm)
		{
			SomethingWeWantToSortReallyFast* itm_upgraded = dynamic_cast<SomethingWeWantToSortReallyFast*>(itm);
			std::cout << "Bal=" << itm_upgraded->m_Balance << " SortIndex=" << itm_upgraded->sortIndex << " Object was added in iteration #" << itm_upgraded->IterationNumber;
			SomethingWeWantToSortReallyFast* left = dynamic_cast<SomethingWeWantToSortReallyFast*>(itm->m_LeftNode);
			SomethingWeWantToSortReallyFast* right = dynamic_cast<SomethingWeWantToSortReallyFast*>(itm->m_RightNode);
			if (left == NULL)
				std::cout << " Left is NULL";
			else
				std::cout << " Left is #" << left->IterationNumber;

			if (right == NULL)
				std::cout << " Right is NULL";
			else
				std::cout << " Right is #" << right->IterationNumber;

			std::cout << "\n";

		}
		);
	};
#endif
	
};

typedef TriTree* PTriTree;
typedef TriTreeItem* PTriTreeItem;

#endif