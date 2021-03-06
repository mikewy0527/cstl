/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 *  This file is part of clib library
 *  Copyright (C) 2011 Avinash Dongre ( dongre.avinash@gmail.com )
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

#include "c_lib.h"

#include <stdio.h>


struct clib_map* 
new_c_map ( clib_compare fn_c_k, clib_destroy fn_k_d,  
            clib_destroy fn_v_d) {

    struct clib_map* pMap  =  (struct clib_map*)malloc(sizeof(struct clib_map));
    if (pMap == (struct clib_map*)0)
        return (struct clib_map*)0;

    pMap->root  = new_c_rb (fn_c_k, fn_k_d, fn_v_d);
    if (pMap->root == (struct clib_rb*)0)
        return (struct clib_map*)0;

    return pMap;
}
clib_error   
insert_c_map ( struct clib_map* pMap, void* key, size_t key_size, void* value,  size_t value_size) {
    if (pMap == (struct clib_map*)0)
        return CLIB_MAP_NOT_INITIALIZED;

    return insert_c_rb ( pMap->root, key, key_size, value, value_size);
}
clib_bool    
exists_c_map ( struct clib_map* pMap, void* key) {
    clib_bool found = clib_false;
    struct clib_rb_node* node;

    if (pMap == (struct clib_map*)0)
        return clib_false;
    
    node = find_c_rb ( pMap->root, key);
    if ( node != (struct clib_rb_node*)0  ) {
        return clib_true;
    }
    return found;    
}
clib_error   
remove_c_map ( struct clib_map* pMap, void* key) {
    clib_error rc = CLIB_ERROR_SUCCESS;
    struct clib_rb_node* node;
    if (pMap == (struct clib_map*)0)
        return CLIB_MAP_NOT_INITIALIZED;

    node = remove_c_rb ( pMap->root, key );
    if ( node != (struct clib_rb_node*)0  ) {
        void* removed_node;
        get_raw_clib_object ( node->key, &removed_node );
        free ( removed_node);
        delete_clib_object ( node->key );

        get_raw_clib_object ( node->value, &removed_node );
        free ( removed_node);
        delete_clib_object ( node->value);

        free ( node );
    }
    return rc;
}
clib_bool    
find_c_map ( struct clib_map* pMap, void* key, void**value) {
    struct clib_rb_node* node;

    if (pMap == (struct clib_map*)0)
        return clib_false;

    node = find_c_rb ( pMap->root, key);
    if ( node == (struct clib_rb_node*)0  ) 
        return clib_false;

    get_raw_clib_object ( node->value, value );

    return clib_true;

}

clib_error    
delete_c_map ( struct clib_map* x) {
    clib_error rc = CLIB_ERROR_SUCCESS;
    if ( x != (struct clib_map*)0 ){
        rc = delete_c_rb ( x->root );
        free ( x );
    }
    return rc;
}

static struct clib_rb_node *
minimum_c_map( struct clib_map *x ) {
	return minimum_c_rb( x->root, x->root->root);
}

static struct clib_object* 
get_next_c_map( struct clib_iterator* pIterator ) {
	if ( ! pIterator->pCurrentElement ) {
		pIterator->pCurrentElement = minimum_c_map(pIterator->pContainer);
	}else {
		struct clib_map *x = (struct clib_map*)pIterator->pContainer;
		pIterator->pCurrentElement = tree_successor( x->root, pIterator->pCurrentElement);			              
	}
	if ( ! pIterator->pCurrentElement)
		return (struct clib_object*)0;

	return ((struct clib_rb_node*)pIterator->pCurrentElement)->value;
}
static void* 
get_value_c_map( void* pObject) {
	void* elem;
	get_raw_clib_object ( pObject, &elem );
	return elem;
}
static void
replace_value_c_map(struct clib_iterator *pIterator, void* elem, size_t elem_size) {
	struct clib_map*  pMap = (struct clib_map*)pIterator->pContainer;
	
	if ( pMap->root->destruct_v_fn ) {
		void* old_element;
		get_raw_clib_object (((struct clib_rb_node*)pIterator->pCurrentElement)->value, &old_element );
		pMap->root->destruct_v_fn(old_element);
    }
	replace_raw_clib_object(((struct clib_rb_node*)pIterator->pCurrentElement)->value, elem, elem_size);
}


struct clib_iterator* 
new_iterator_c_map(struct clib_map* pMap) {
	struct clib_iterator *itr = ( struct clib_iterator*) malloc ( sizeof ( struct clib_iterator));
	itr->get_next     = get_next_c_map;
	itr->get_value    = get_value_c_map;
	itr->replace_value = replace_value_c_map;
	itr->pContainer   = pMap;
	itr->pCurrent     = 0;
	itr->pCurrentElement = (void*)0;
	return itr;
}
void
delete_iterator_c_map ( struct clib_iterator* pItr) {
	free ( pItr );
}
