#pragma once
#include "body.h"

namespace crystal {
	/**
	* Stores a potential contact to check later.
	*/
	struct PotentialContact
	{
		RigidBody* body[2];
	};

	/**
	* Represents a bounding sphere that can be tested for overlap.
	*/
	struct BoundingSphere
	{
	public:
		Vector3 center;
		real radius;

		BoundingSphere(const Vector3& center, real radius) :center(center), radius(radius) {}
		
		/**
		* Creates a bounding sphere to enclose the two given bounding
		* spheres.
		*/
		BoundingSphere(const BoundingSphere& bs1, const BoundingSphere& bs2);

		bool overlaps(const BoundingSphere& other) const;

		/**
		* Returns the volume of this bounding volume. This is used
		* to calculate how to recurse into the bounding volume tree.
		* For a bounding sphere it is a simple calculation.
		*/
		real getSize() const
		{
			return ((real)1.333333) * R_PI * radius * radius * radius;
		}

		/**
		* Reports how much this bounding sphere would have to grow
		* by to incorporate the given bounding sphere. Note that this
		* calculation returns a value not in any particular units (i.e.
		* its not a volume growth). In fact the best implementation
		* takes into account the growth in surface area (after the
		* Goldsmith-Salmon algorithm for tree construction).
		*/
		real getGrowth(const BoundingSphere& other) const;
	};

	/**
	* A base class for nodes in a bounding volume hierarchy.
	*
	* This class uses a binary tree to store the bounding
	* volumes.
	*/
	template<class BoundingVolumeClass>
	class BVHNode
	{
	public:
		/*Holds a single bounding volume encompassing all the
		 * descendents of this node.
		 */
		BoundingVolumeClass volume;

		/**
		* Holds the rigid body at this node of the hierarchy.
		* Only leaf nodes can have a rigid body defined (see isLeaf).
		* Note that it is possible to rewrite the algorithms in this
		* class to handle objects at all levels of the hierarchy,
		* but the code provided ignores this vector unless firstChild
		* is NULL.
		*/
		RigidBody* body;

		//direct parent node
		BVHNode* parent;

		//child nodes. NULL for both if the node is leaf node
		BVHNode* chidren[2];

		BVHNode(BVHNode* parent,BoundingVolumeClass& volume,RigidBody* body = NULL)
			:volume(volume), body(body), parent(parent)
		{
			chidren[0] = chidren[1] = NULL;
		}

		~BVHNode();

		/**
		* Checks if this node is at the bottom of the hierarchy.
		*/
		bool isLeaf() const
		{
			return body != NULL;
		}

		/**
		* Checks the potential contacts from this node downwards in
		* the hierarchy, writing them to the given array (up to the
		* given limit). Returns the number of potential contacts it
		* found.
		*/
		unsigned getPotentialContacts(PotentialContact* contacts,
			unsigned limit) const;

		/**
		* Inserts the given rigid body, with the given bounding volume,
		* into the hierarchy. This may involve the creation of
		* further bounding volume nodes.
		*/
		void insert(const BoundingVolumeClass& newVolume, const RigidBody* const newBody);

		/**
		* For non-leaf nodes, this method recalculates the bounding volume
		* based on the bounding volumes of its children.
		*/
		void recalculateBoundingVolume(bool recurse = true);

	protected:
       /**
		* Checks for overlapping between nodes in the hierarchy. Note
		* that any bounding volume should have an overlaps method implemented
		* that checks for overlapping with another object of its own type.
		*/
		bool overlaps(const BVHNode<BoundingVolumeClass>* other) const;

		/**
		* Checks the potential contacts between this node and the given
		* other node, writing them to the given array (up to the
		* given limit). Returns the number of potential contacts it
		* found.
		*/
		unsigned getPotentialContactsWith(
			const BVHNode<BoundingVolumeClass> *other,
			PotentialContact* contacts,
			unsigned limit) const;
	};

	template<class BoundingVolumeClass>
	unsigned BVHNode<BoundingVolumeClass>::getPotentialContacts(PotentialContact* contacts, unsigned limit) const
	{
		if (isLeaf() || limit == 0) return 0;
		return chidren[0]->getPotentialContactsWith(chidren[1], contacts, limit);
	}

	template<class BoundingVolumeClass>
	bool BVHNode<BoundingVolumeClass>::overlaps(const BVHNode<BoundingVolumeClass>* other) const
	{
		return volume->overlaps(other->volume);
	}

	template<class BoundingVolumeClass>
	BVHNode<BoundingVolumeClass>::~BVHNode()
	{
		// If we don't have a parent, then we ignore the sibling
		// processing
		if (parent)
		{
			// Find our sibling
			BVHNode<BoundingVolumeClass> *sibling;
			if (parent->children[0] == this) sibling = parent->children[1];
			else sibling = parent->children[0];

			// Write its data to our parent
			parent->volume = sibling->volume;
			parent->body = sibling->body;
			parent->children[0] = sibling->children[0];
			if(sibling->chidren[0]) sibling->chidren[0]->parent = parent;
			parent->children[1] = sibling->children[1];
			if (sibling->chidren[1]) sibling->chidren[1]->parent = parent;
			// Delete the sibling (we blank its parent and
			// children to avoid processing/deleting them)
			sibling->parent = NULL;
			sibling->body = NULL;
			sibling->children[0] = NULL;
			sibling->children[1] = NULL;
			delete sibling;

			// Recalculate the parent's bounding volume
			parent->recalculateBoundingVolume();
		}

		// Delete our children (again we remove their
		// parent data so we don't try to process their siblings
		// as they are deleted).
		if (children[0]) {
			children[0]->parent = NULL;
			delete children[0];
		}
		if (children[1]) {
			children[1]->parent = NULL;
			delete children[1];
		}
	}

	template<class BoundingVolumeClass>
	void BVHNode<BoundingVolumeClass>::insert(const BoundingVolumeClass& newVolume, const RigidBody* const newBody)
	{
		//If this is leaf node, simply append a new node as its subling and add a parent
		if (this->isLeaf())
		{
			children[0] = new BVHNode{ this,volume,body };
			children[1] = new BVHNode{this,newVolume,newBody};
			this->body = NULL;
			// We need to recalculate our bounding volume
			recalculateBoundingVolume();
		}
		// Otherwise we need to work out which child gets to keep
		// the inserted body. We give it to whoever would grow the
		// least to incorporate it.
		else
		{
			if (children[0]->volume.getGrowth(newVolume)
				> children[1]->volume.getGrowth(newVolume))
			{
				children[1]->insert(newVolume, newBody);
			}
			else
			{
				children[0]->insert(newVolume, newBody);
			}
		}
	}

	template<class BoundingVolumeClass>
	void BVHNode<BoundingVolumeClass>::recalculateBoundingVolume(
		bool recurse
	)
	{
		if (isLeaf()) return;

		// Use the bounding volume combining constructor.
		volume = BoundingVolumeClass(
			children[0]->volume,
			children[1]->volume
		);

		// Recurse up the tree
		if (parent) parent->recalculateBoundingVolume(true);
	}

	template<class BoundingVolumeClass>
	unsigned BVHNode<BoundingVolumeClass>::getPotentialContactsWith(
		const BVHNode<BoundingVolumeClass> *other, PotentialContact* contacts,
		unsigned limit) const
	{
		//no contact or reach limit
		if (!overlaps(other) || limit <= 0) return;

		//both leaf nodes. Record contact
		if (isLeaf() && other->isLeaf())
		{
			contacts->body[0] = body;
			contacts->body[1] = other->body;
			return 1;
		}

		// Determine which node to descend into. If either is
		// a leaf, then we descend the other. If both are branches,
		// then we use the one with the largest size.
		if (other->isLeaf() || ((!isLeaf()) && volume -> getSize() >= other->volume->getSize()))
		{
			//Descend into this node
			unsigned count = children[0]->getPotentialContactsWith(other,contacts,limit);
			//Check we have enough slots to do the other side too
			if (limit > count)
			{
				return count + chidren[1]->getPotentialContactsWith(other, contacts+count, limit - count);
			}
			else
			{
				return count;
			}
		}
		else 
		{
			// Recurse into the other node
			unsigned count = getPotentialContactsWith(other->chidren[0],contacts,limit);

			if (limit > count)
			{
				return count + getPotentialContactsWith(other->chidren[1], contacts + count, limit - count);
			}
			else
			{
				return count;
			}
		}

	}

}
