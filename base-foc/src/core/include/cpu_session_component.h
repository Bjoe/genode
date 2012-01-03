/*
 * \brief  Core-specific instance of the CPU session/thread interfaces
 * \author Christian Helmuth
 * \author Stefan Kalkowski
 * \date   2006-07-17
 */

/*
 * Copyright (C) 2006-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _CORE__INCLUDE__CPU_SESSION_COMPONENT_H_
#define _CORE__INCLUDE__CPU_SESSION_COMPONENT_H_

/* Genode includes */
#include <util/list.h>
#include <base/allocator_guard.h>
#include <base/tslab.h>
#include <base/lock.h>
#include <base/pager.h>
#include <base/rpc_server.h>
#include <foc_cpu_session/foc_cpu_session.h>

/* core includes */
#include <platform_thread.h>

namespace Genode {

	/**
	 * RPC interface of CPU thread
	 *
	 * We make 'Cpu_thread' a RPC object only to be able to lookup CPU threads
	 * from thread capabilities supplied as arguments to CPU-session functions.
	 * A CPU thread does not provide an actual RPC interface.
	 */
	struct Cpu_thread
	{
		GENODE_RPC_INTERFACE();
	};


	class Cpu_thread_component : public Rpc_object<Cpu_thread>,
	                             public List<Cpu_thread_component>::Element
	{
		private:

			Platform_thread _platform_thread;

			bool            _bound;            /* pd binding flag */

		public:

			Cpu_thread_component(const char *name, unsigned priority)
			: _platform_thread(name, priority), _bound(false) { }


			/************************
			 ** Accessor functions **
			 ************************/

			inline Platform_thread * platform_thread() { return &_platform_thread; }
			inline bool bound() const                  { return _bound; }
			inline void bound(bool b)                  { _bound = b; }
	};


	class Cpu_session_component : public Rpc_object<Foc_cpu_session>
	{
		private:

			/**
			 * Allocator used for managing the CPU threads associated with the
			 * CPU session
			 */
			typedef Tslab<Cpu_thread_component, 1024> Cpu_thread_allocator;

			Rpc_entrypoint            *_thread_ep;
			Pager_entrypoint          *_pager_ep;
			Allocator_guard            _md_alloc;          /* guarded meta-data allocator */
			Cpu_thread_allocator       _slab;              /* meta-data allocator */
			Lock                       _slab_lock;         /* protect slab access */
			List<Cpu_thread_component> _thread_list;
			Lock                       _thread_list_lock;  /* protect thread list */
			unsigned                   _priority;          /* priority of threads
			                                                  created with this
			                                                  session */

			/**
			 * Lookup thread in CPU session by its capability
			 *
			 * \retval NULL  thread capability is invalid or
			 *               does not belong to the CPU session
			 */
			Cpu_thread_component *_lookup_thread(Thread_capability thread) {
				return dynamic_cast<Cpu_thread_component *>
				       (_thread_ep->obj_by_cap(thread)); }

			/**
			 * Raw thread-killing functionality
			 *
			 * This function is called from the 'kill_thread' function and
			 * the destructor. Each these functions grab the list lock
			 * by themselves and call this function to perform the actual
			 * killing.
			 */
			void _unsynchronized_kill_thread(Cpu_thread_component *thread);

		public:

			/**
			 * Constructor
			 */
			Cpu_session_component(Rpc_entrypoint   *thread_ep,
			                      Pager_entrypoint *pager_ep,
			                      Allocator *md_alloc, const char *args);

			/**
			 * Destructor
			 */
			~Cpu_session_component();


			/***************************
			 ** CPU session interface **
			 ***************************/

			Thread_capability create_thread(Name const &);
			void kill_thread(Thread_capability);
			Thread_capability first();
			Thread_capability next(Thread_capability);
			int set_pager(Thread_capability, Pager_capability);
			int start(Thread_capability, addr_t, addr_t);
			void pause(Thread_capability thread_cap);
			void resume(Thread_capability thread_cap);
			void single_step(Thread_capability thread_cap, bool enable);
			void cancel_blocking(Thread_capability);
			int name(Thread_capability, char *, size_t);
			int state(Thread_capability, Thread_state *);
			void exception_handler(Thread_capability, Signal_context_capability);


			/***********************************
			 ** Fiasco.OC specific extensions **
			 ***********************************/

			void enable_vcpu(Thread_capability, addr_t);
			Native_capability native_cap(Thread_capability);
			Native_capability alloc_irq();
	};
}

#endif /* _CORE__INCLUDE__CPU_SESSION_COMPONENT_H_ */
