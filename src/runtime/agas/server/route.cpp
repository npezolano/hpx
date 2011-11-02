////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2011 Vinay C Amatya
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#include <hpx/runtime/actions/continuation_impl.hpp>
#include <hpx/runtime/agas/server/primary_namespace.hpp>

namespace hpx { namespace agas { namespace server
{

bool primary_namespace::route(
    parcelset::parcel const& p
  , error_code& ec
    )
{
    // TODO: protection of parcel's content?
    naming::gid_type id = p.get_destination();
    naming::strip_credit_from_gid(id);

    request req(primary_ns_resolve_gid, id);

    response rep = service(req, ec);

    if (ec || success != rep.get_status())
        return false;

    // Resolve the gva to the real resolved address (which is just a gva
    // with as fully resolved LVA and an offset of zero).
    gva const g = rep.get_gva().resolve(p.get_destination()
                                      , rep.get_base_gid());

    naming::address addr;

    addr.locality_ = g.endpoint;
    addr.type_ = g.type;
    addr.address_ = g.lva();

    parcelset::parcel p_temp = p;
    p_temp.set_destination(id);
    p_temp.set_destination_addr(addr);

    // Assign the parcel to parcelhandler.
    get_runtime().get_parcel_handler().put_parcel(p_temp);

    // TODO: update sender's cache for the destination id rosolution
    return true;
}

}}}
