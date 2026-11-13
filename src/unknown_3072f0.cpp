// @flags /O2 /GL-
// Havok (third-party physics library): bringing a box pair's kept contacts
// up to date: each kept feature (a corner of one box against a face of the
// other, or two edges) is worked out again, and passed on as a contact while
// it still lies within the bounds; otherwise its id goes back to the owner
// and it leaves the manifold.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"

static __declspec(align(16)) const unsigned long absolute_mask_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

struct s_308180_entry
{
	unsigned short first;
	unsigned short second;
	unsigned short field_4;
};

// a manifold of up to eight contacts (0x308180)
class class_308180
{
public:
	char unknown0[0x20];
	s_308180_entry entries[8];
	long field_50;
	int count;
	bool changed;

	void remove(int index, long a, long b);
};

// the closest points of two features, a scale, their distance, and the
// features' keys
struct feature_3072f0
{
	__m128 first_point;
	__m128 second_point;
	__m128 normal;
	float scale;
	float distance;
	unsigned short kind;
	unsigned short field_3a;
};

struct bool_3058b0
{
	bool value;
};

struct corner_305bf0;
struct corner_key_305bf0;
struct corner_305d40;
struct corner_key_305d40;
struct contact_305ca0;
struct key_305ca0;
struct feature_305ca0;
struct contact_305e10;
struct key_305e10;
struct feature_305e10;
struct contact_305eb0;
struct key_305eb0;
struct feature_305eb0;
struct feature_305f60;

class box_corner_305bf0
{
public:
	void function_305bf0(corner_305bf0 *out, corner_key_305bf0 const *key) const;
};

class box_pair_305d40
{
public:
	void function_305d40(corner_305d40 *out, corner_key_305d40 const *key) const;
};

class pair_305ca0
{
public:
	void function_305ca0(contact_305ca0 *out, key_305ca0 const *key, feature_305ca0 const *feature) const;
};

class pair_305e10
{
public:
	void function_305e10(contact_305e10 *out, key_305e10 const *key, feature_305e10 const *feature) const;
};

class pair_305eb0
{
public:
	void function_305eb0(contact_305eb0 *out, key_305eb0 const *key, feature_305eb0 const *feature) const;
};

class pair_305f60
{
public:
	bool_3058b0 function_306f80(feature_305f60 *feature);
};

struct contact_3072f0
{
	char unknown0[0x30];
};

ELEMENTS_2D91D0_ALIGNED(contacts_3072f0, contact_3072f0);

struct results_3072f0
{
	contacts_3072f0 *contacts;
};

class owner_3072f0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void release_id(unsigned short id) {}
};

// a pair of boxes: their half extents and the bounds a kept feature must
// stay within
class box_pair_3072f0
{
public:
	long field_0;
	long field_4;
	long field_8;
	owner_3072f0 *owner;
	results_3072f0 *results;
	char unknown14[0x20 - 0x14];
	__m128 first_extents;
	__m128 second_extents;
	__m128 first_bounds;
	__m128 second_bounds;

	void function_3072f0(class_308180 *manifold);
};

// score 97 (was 107): retail keeps the manifold in ebx and the entry pointer
// in edi, ours keeps the entry pointer in ebx and the manifold in edi.
// The passing test's arm first 97; array reference 107; array reference member 107;
// contact reference 107; direct append argument 107; a named test result 129;
// output and array references 107; both array and contact references 107;
// manifold object reference 107; array read before distance 101; typed contact 97;
// shared validity result 170; distinct record names 97; direct key tests 180.
// Bounds-pass arms first 97, second 97, both 97; vector members 97;
// long index 97; records at top 97; scalar distance locals 97;
// byte masks 99; placement after the first callee 97.
// Further trials: array reference with the positive edge arm 97; nested
// output/array/contact references 97; direct append argument in that arm 97;
// const member 97; index renamed 97; iterator renamed 97; global
// optimization disabled 781; size preference 220. Opening body retained.
// Stored array reference 97; current entry from index 193; direct indexed
// entry uses 193; cursor in for scope 96; ordinal declared first 96; while
// loop 97; one int for kind 182; retained entry-array base 97. NOT kept:
// the 96 forms gain only one. Opening executable source restored.
// @retail 0x3072f0
void box_pair_3072f0::function_3072f0(class_308180 *manifold)
{
	s_308180_entry *entry = manifold->entries;
	for (int i = 0; i < manifold->count;)
	{
		unsigned short const kind = entry->first;
		int const type = kind;
		if (type <= 2)
		{
			feature_3072f0 feature;
			((box_corner_305bf0 const *)this)->function_305bf0((corner_305bf0 *)&feature, (corner_key_305bf0 const *)entry);
			if ((_mm_movemask_ps(_mm_cmplt_ps(_mm_and_ps(feature.first_point, *(__m128 const *)absolute_mask_455940), first_bounds)) & 7) != 7)
			{
				goto drop;
			}
			int const axis = feature.kind;
			feature.distance = feature.scale * feature.first_point.m128_f32[axis] - first_extents.m128_f32[axis];
			contact_3072f0 *contact = &results->contacts->next_slot();
			((pair_305ca0 const *)this)->function_305ca0((contact_305ca0 *)contact, (key_305ca0 const *)entry, (feature_305ca0 const *)&feature);
		}
		else if (type <= 5)
		{
			feature_3072f0 feature;
			((box_pair_305d40 const *)this)->function_305d40((corner_305d40 *)&feature, (corner_key_305d40 const *)entry);
			if ((_mm_movemask_ps(_mm_cmplt_ps(_mm_and_ps(feature.second_point, *(__m128 const *)absolute_mask_455940), second_bounds)) & 7) != 7)
			{
				goto drop;
			}
			int const axis = feature.kind - 3;
			feature.distance = -(feature.scale * feature.second_point.m128_f32[axis]) - second_extents.m128_f32[axis];
			contact_3072f0 *contact = &results->contacts->next_slot();
			((pair_305e10 const *)this)->function_305e10((contact_305e10 *)contact, (key_305e10 const *)entry, (feature_305e10 const *)&feature);
		}
		else
		{
			feature_3072f0 feature;
			feature.kind = kind;
			feature.field_3a = entry->second;
			if (((pair_305f60 *)this)->function_306f80((feature_305f60 *)&feature).value)
			{
				contact_3072f0 *contact = &results->contacts->next_slot();
				((pair_305eb0 const *)this)->function_305eb0((contact_305eb0 *)contact, (key_305eb0 const *)entry, (feature_305eb0 const *)&feature);
			}
			else
			{
				goto drop;
			}
		}
		i++;
		entry++;
		continue;
	drop:
		if (entry->first <= 5)
		{
			manifold->field_50--;
		}
		owner->release_id(manifold->entries[i].field_4);
		manifold->remove(i, field_0, field_4);
	}
}
