// UT_Tree.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "windows.h"
#include "commonx\typex.h"
#include "commonx\tree.h"
#include "commonx\sharedobject.h"
#include <iostream>
#include <signal.h>

//*******************************************************
//This converts Access violation into a C++ Exception
//It is hooked in the first line of main()
void SignalHandler(int signal)
{
	throw std::exception("!Access Violation!");
}


//*******************************************************
// When there's something we want to sort, we can easily inherit from 
// TriTreeItem.  In some instances, I use "Linkage" objects that inherit from TriTreeItem
// so the items are sorted indirectly via the use of Linkages
// that are attached to other objects, particularly in the 
// case where an object may have multiple indexes.
// I'm keeping this demo simple, for now.
class SomethingWeWantToSortReallyFast : public TriTreeItem
{
public:
	//constructor-----------------
	SomethingWeWantToSortReallyFast() : sortIndex(0) {};
	~SomethingWeWantToSortReallyFast() {};
	NativeInt sortIndex;
	NativeInt IterationNumber;

	//REQUIRED*: Compare is ABSTRACT in base class, you must
	//provide a Compare function that compares this object to it's siblings.
	//There is the potential for the tree to be heterogeneous, so 
	//in those cases, you can check if of the CompareTo parameter is of
	//various types and upgrade it
	ni Compare(TriTreeItem* CompareTo)
	// CompareTo < this :-1  CompareTo=this :0  CompareTo > this :+1
	{
		//we have to upgrade the ancestor class to compare
		//potentially this could be enhanced with some Type/template parameters.
		SomethingWeWantToSortReallyFast* OtherThing = dynamic_cast<SomethingWeWantToSortReallyFast*>(CompareTo);
		if (OtherThing->sortIndex < this->sortIndex) return -1;
		if (OtherThing->sortIndex > this->sortIndex) return 1;
		return 0;//if equal
	}
};


#define SLEEP_TIME 50



//***********************************************************
//MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN
int main()
{	
	signal(SIGSEGV, SignalHandler);	//TRAP Access Violations and convert to C++ exception class
	try
	{
		//SOME LONG-WINDED TEXT WRITTEN BY SOME BLOWHARD
		system("CLS");
		std::cout << "The AVLTree is widely considered the \"Holy Grail\" of random access sorting and indexing\n"; Sleep(SLEEP_TIME);
		std::cout << "Its lookup, insert, and delete times are all ~log(n) and protected from imbalance.\n"; Sleep(SLEEP_TIME);
		std::cout << "To iterate the tree, the tree is walked leftnode, currentnode, rightnode \n\n"; Sleep(SLEEP_TIME);
		std::cout << "The AVLTree has a limitation in that it is unable to store items with duplicate key values...\n... and defers the problem of maintaining duplicates to the caller.\n\n"; Sleep(SLEEP_TIME);
		std::cout << "My TriTree class solves this problem.\n\n";
		std::cout << "Press enter to continue...\n"; getchar();

		system("CLS");
		std::cout << "...The TriTree class...\n\n";
		std::cout << "This tree is walked leftnode, currentnode, lowernode(s), rightnode, \n although this implementation has a dirty secret.\n\n"; Sleep(SLEEP_TIME);
		std::cout << "Speed is typically still ~log(n), although it depends on the number of duplicates in some cases.\n"; (SLEEP_TIME);
		std::cout << "\nI've implemented the Iterate() function to use lambdas so that you don't have to walk the tree manually.\nThe complexities are hidden behind a simple interface.\n"; Sleep(SLEEP_TIME);
		std::cout << "\nAlso the pointers to objects added to this tree are valid to external referencers. Most AVL implementations you'll find\n do not offer this.\n"; Sleep(SLEEP_TIME);
		std::cout << "\nPress enter to continue...\n"; getchar();

		system("CLS");
		std::cout << "\nNow for DEMO #1\n\n"; Sleep(SLEEP_TIME);
		std::cout << "Here we're going to sort a class by an index value that does NOT have to be unique.\nSorting order for duplicates is arbitrary.\n"; Sleep(SLEEP_TIME);
		std::cout << "\nWe'll do a quick test, creating 25 objects and adding them to 9 sorted buckets, 1 thru 9.\n"; Sleep(SLEEP_TIME);
		std::cout << "\nThen we'll WALK the tree with the simple Iterate() function and print the results.\n"; Sleep(SLEEP_TIME);
		std::cout << "\nPress enter to continue...\n"; getchar();
		system("CLS");

		{
			//************BEGIN IMPORTANT STUFF
			TriTree tree;

			//demo values for the sort index... duplicates are intentional
			const int demo_values[] = { 4, 1, 2, 3, 4, 5, 7, 8, 9, 5, 4, 3, 6, 7, 5, 4, 3, 2, 3, 4, 5, 6, 7, 8, 9 };

			//ADD 25 objects to the tree
			for (int t = 0; t < 25; t++)
			{
				SomethingWeWantToSortReallyFast* fastobj = new SomethingWeWantToSortReallyFast();
				fastobj->sortIndex = demo_values[t];//there will be duplicates of this!
				fastobj->IterationNumber = t;//we'll tag each object uniquely with the iteration# so that we can see where they ultimately end up!
				tree.Add(fastobj);
			}

			//now PRINT the sorted results
			tree.Iterate(
				[=](PTriTreeItem itm)
			{
				SomethingWeWantToSortReallyFast* itm_upgraded = dynamic_cast<SomethingWeWantToSortReallyFast*>(itm);
				std::cout << "SortIndex=" << itm_upgraded->sortIndex << " Object was added in iteration #" << itm_upgraded->IterationNumber << "\n";
			}
			);

			SomethingWeWantToSortReallyFast* firstitem = dynamic_cast<SomethingWeWantToSortReallyFast*>(tree.FirstItem());
			std::cout << "FirstItem() returns SortIndex=" << firstitem->sortIndex << " iteration #" << firstitem->IterationNumber << "(any item with lowest sort index by definition)\n";
			SomethingWeWantToSortReallyFast* lastitem = dynamic_cast<SomethingWeWantToSortReallyFast*>(tree.LastItem());
			std::cout << "LastItem() returns SortIndex=" << lastitem->sortIndex << " iteration #" << lastitem->IterationNumber << " (any item with highest sort index by definition)\n";



			//*********************************
		}

		std::cout << "\nIf I did my job well, you should have the objects listed by sort-index, NOT the iteration#...\n... and the interface for this is DIRT SIMPLE... ";
		std::cout << "\nPress enter to continue...\n"; getchar();

		//#################################################################################################			
#define ADD_COUNT 1000000
#define BUCKET_COUNT 1000000
		system("CLS");
		std::cout << "DEMO #2\nNow let's see how fast it can go by adding and sorting " << ADD_COUNT << " objects into " << BUCKET_COUNT << " random buckets.";
		std::cout << "\n\nPress enter to continue...\n"; getchar();
		int64_t tmDeleteStart=0;//scoped for later use
		{
			//************BEGIN IMPORTANT STUFF
			TriTree tree;

			//demo values for the sort index... duplicates are intentional
			int64_t tmStart = GetTickCount64();

			//ADD objects to the tree
			for (int t = 0; t < ADD_COUNT; t++)
			{
				SomethingWeWantToSortReallyFast* fastobj = new SomethingWeWantToSortReallyFast();
				fastobj->sortIndex = rand() % BUCKET_COUNT;//there will be duplicates of this!
				fastobj->IterationNumber = t;//we'll tag each object uniquely with the iteration# so that we can see where they ultimately end up!
				tree.Add(fastobj);
				//DebugTree(&tree);
				//std::cout << "Press enter to continue...\n"; getchar();
			}
			//*********************************

			int64_t tmEnd = GetTickCount64();

			std::cout << "We built a sorted list of " << BUCKET_COUNT << " items in " << (tmEnd - tmStart) << "ms!";
			std::cout << "\nIf you would like to check each item individually. Continue on!";
			std::cout << "\n\nPress enter to continue...\n"; getchar();

			//now PRINT the sorted results
			NativeInt iter = 0;
			tree.Iterate(
				[=, &iter](PTriTreeItem itm, bool& needstop)
			{
				SomethingWeWantToSortReallyFast* itm_upgraded = dynamic_cast<SomethingWeWantToSortReallyFast*>(itm);
				std::cout << "SortIndex=" << itm_upgraded->sortIndex << " Object was added in iteration #" << itm_upgraded->IterationNumber << "\n";
				iter++;
				needstop = iter == 1000;
				if (needstop)
				{
					std::cout << "We'll stop at 1000 items assuming that you don't want to sit here all day.\n";
				}
			}
			);

			std::cout << "\n\nReminder: Duplicate sort indexes are a feature (not a bug), and are expected in this test.\n";

			std::cout << "Next. the Items in the tree will be removed and deleted.\n";
			std::cout << "\n\nPress enter to continue...\n"; getchar();

			//**********delete timing test
			tmDeleteStart = GetTickCount64();
		}  //tree goes out of scope here

		int64_t tmDeleteEnd = GetTickCount64();
		//end of delete timing

		std::cout << "Tree deleted in " << (tmDeleteEnd - tmDeleteStart) << "ms.\n";
		std::cout << "\n\nPress enter to continue...\n"; getchar();



	}
	catch (std::exception e)
	{
		std::cout << "Unhandled Exception: " << e.what() << "\n";
	}
	catch (...)
	{
		std::cout << "Unhandled Exception!\n";
	}		
	std::cout << "End of Program!\n";
	
}

