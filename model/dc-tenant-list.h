/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef __DC_TENANT_LIST_H__
#define __DC_TENANT_LIST_H__

#include <vector>
#include "ns3/ptr.h"

namespace ns3 {

class DCTenant;
class DCVm;

/**
 * \ingroup network
 *
 * \brief the list of datacenter nodes.
 *
 * Every DCTenant created is automatically added to this list.
 */
class DCTenantList
{
public:
  typedef std::vector<Ptr<DCTenant> >::const_iterator Iterator;

  /**
   * \param node node to add
   * \returns index of node in list.
   *
   * This method is called automatically from DCTenant::DCTenant so
   * the user has little reason to call it himself.
   */
  static uint32_t Add (Ptr<DCTenant> node);

  /**
   * \returns a C++ iterator located at the beginning of this
   *          list.
   */
  static Iterator Begin (void);
  /**
   * \returns a C++ iterator located at the end of this
   *          list.
   */
  static Iterator End (void);
  /**
   * \param n index of requested node.
   * \returns the DCTenant associated to index n.
   */
  static Ptr<DCTenant> GetDCTenant (uint32_t n);
  /**
   * \returns the number of nodes currently in the list.
   */
  static uint32_t GetNDCTenants (void);

  static Ptr<DCTenant> GetDCTenant(Ptr<DCVm> vm);
};

} // namespace ns3


#endif /* __DC_TENANT_LIST_H__ */

