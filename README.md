# TriTree
A 3 legged tree, for when binary trees just aren't enough.

Look for main() in UT_Tree.cpp

This is a cheesy demo app that demonstrates an AVL-Tree variant I created.  The primary difference between this and a typical AVL-Tree is that it has 3 legs, not two.  The CENTER leg is used to the storage of duplicates and behaves much like a linked-list.  


# why did I build this?

I like my classes to be universally useful whenever possible.  A traditional AVL-Tree is a world-class solution to the random-access sorted list problem, however, it is NOT universally useful for the simple reason that it cannot contain duplicate key values.  By adding this capability without changing the interface, we are making this class universally useful... and we now have a one-size-fits most tool in our toolbelts.

# coding standards
You all probably have stronger opinions about C++ coding standards than I do, but usually my opinions about C++ coding standards revolve around techniques to drag it out of the 70s.  

The implementation of this tree is complex and hard to follow.  It is almost like trying to solve a rubick's cube in parts.  But none of the internal complexities of the tree are all that important... because what is more important than the implementation is the way you interact with it.  This class takes a very complex concept and buries it inside a very simple interface. With this in mind, the Triree class exposes iterators through Lambda functions.  

It is super easy to add items to the tree
			
      TriTree tree;			
			const int demo_values[] = { 4, 1, 2, 3, 4, 5, 7, 8, 9, 5, 4, 3, 6, 7, 5, 4, 3, 2, 3, 4, 5, 6, 7, 8, 9 };

			//ADD 25 objects to the tree
			for (int t = 0; t < 25; t++)
			{
				SomethingWeWantToSortReallyFast* fastobj = new SomethingWeWantToSortReallyFast();
				fastobj->sortIndex = demo_values[t];//there will be duplicates of this!
				fastobj->IterationNumber = t;//we'll tag each object uniquely with the iteration# so that we can see where they ultimately end up!
				tree.Add(fastobj);
			}

It is also super easy to iterate the tree in sorted order

      //now PRINT the sorted results
			tree.Iterate(
				[=](PTriTreeItem itm)
			{
				SomethingWeWantToSortReallyFast* itm_upgraded = dynamic_cast<SomethingWeWantToSortReallyFast*>(itm);
				std::cout << "SortIndex=" << itm_upgraded->sortIndex << " Object was added in iteration #" << itm_upgraded->IterationNumber << "\n";
			}
			);

I take similar "one ring to rule them all" type approaches with other classes.  In the future I'll cover Threads (with Thread Pooling) including Commands, Queues, and Anonymous background functions, and maybe move onto FileStreams.






