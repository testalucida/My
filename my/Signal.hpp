#ifndef _SIGNAL_HPP_
#define _SIGNAL_HPP_

#include <cassert>

namespace my {

struct NULLPARM {};

template < class SigSrc, class SigParm >
class Signal {
	typedef void (*callbackDelegatePtr)(SigSrc *, void* pSlot, SigParm*);
public:
	Signal() : _cbcnt(0) {}

	/** connect to signal */
	template < class Slot, void (Slot::*Callback)(SigSrc&, SigParm&) >
	void connect( Slot* pSlot ) {
		assert( _cbcnt < MAX_CB );
		_callbacks[_cbcnt].pSlot = pSlot;
		_callbacks[_cbcnt].pCallbackDelegate = &callbackDelegate < Slot, Callback >;
		_cbcnt++;
	}

	/** send signal to the connected callbacks */
	void send( SigSrc &sigSrc, SigParm &sigParm ) {
		//call each connected callback:
		for( int i = 0; i < _cbcnt; i++ ) {
			(*_callbacks[i].pCallbackDelegate)( &sigSrc, _callbacks[i].pSlot, &sigParm );
		}
	}

	/** send signal to the connected callbacks */
	void send( SigSrc *pSigSrc, SigParm *pSigParm ) {
		//call each connected callback:
		for( int i = 0; i < _cbcnt; i++ ) {
			(*_callbacks[i].pCallbackDelegate)( pSigSrc, _callbacks[i].pSlot, pSigParm );
		}
	}

	/** disconnect callback from signal */
	template < class Slot, void (Slot::*Callback)( SigSrc&, SigParm& ) >
	void disconnect( Slot* pSlot ) {
		for( int i = 0; i < _cbcnt; i++ ) {
			if( _callbacks[i].pSlot == pSlot &&
				_callbacks[i].pCallbackDelegate == &callbackDelegate < Slot, Callback > )
			{
				removeConnectedCallback( i );
			}
		}
	}

    /** disconnect all callbacks of pSlot */
    void disconnectAll( void *pSlot ) {
        for( int i = 0; i < _cbcnt; i++ ) {
			if( _callbacks[i].pSlot == pSlot ) {
				removeConnectedCallback( i );
			}
		}
    }

	/** provides number of callbacks */
	int getConnectionCount() {
		return _cbcnt;
	}

private:
	/** does the callback */
	template < class Slot, void (Slot::*Callback)(SigSrc&, SigParm&) >
	static void callbackDelegate( SigSrc *pSigSrc, void* pSlot, SigParm *pSigParm )  {
		Slot* p = static_cast< Slot* >( pSlot );
		(p->*Callback)( *pSigSrc, *pSigParm );
	}

	void removeConnectedCallback( int idx ) {
		for( int i = idx+1; i < _cbcnt; i++ ) {
			_callbacks[i-1] = _callbacks[i];
		}
		_cbcnt--;
	}

private:
	//struct containing listener class ("Slot") and
	//pointer to callback method
	struct connectedCallback {
		void *pSlot;
		callbackDelegatePtr pCallbackDelegate;
	};
	//max cnt of connected callback:
	static int const MAX_CB = 1000;
	//array of connected callbacks:
	connectedCallback _callbacks[MAX_CB];
	//cnt of actually connected callbacks:
	int _cbcnt;
};

} //ns my

#endif
