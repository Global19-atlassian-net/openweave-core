/*
 *
 *    Copyright (c) 2018 Nest Labs, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef GENERIC_NETWORK_PROVISIONING_SERVER_IMPL_H
#define GENERIC_NETWORK_PROVISIONING_SERVER_IMPL_H

#include <Weave/DeviceLayer/internal/WeaveDeviceLayerInternal.h>
#include <Weave/Profiles/network-provisioning/NetworkProvisioning.h>

namespace nl {
namespace Weave {
namespace DeviceLayer {
namespace Internal {

class NetworkProvisioningServerImpl;
class DeviceNetworkInfo;

template<class ImplClass>
class GenericNetworkProvisioningServerImpl
    : public ::nl::Weave::Profiles::NetworkProvisioning::NetworkProvisioningServer,
      public ::nl::Weave::Profiles::NetworkProvisioning::NetworkProvisioningDelegate
{
protected:

    using ServerBaseClass = ::nl::Weave::Profiles::NetworkProvisioning::NetworkProvisioningServer;
    using NetworkInfo = ::nl::Weave::DeviceLayer::Internal::DeviceNetworkInfo;
    using NetworkType_t = ::nl::Weave::Profiles::NetworkProvisioning::NetworkType;
    using PacketBuffer = ::nl::Weave::System::PacketBuffer;
    using WiFiSecurityType_t = ::nl::Weave::Profiles::NetworkProvisioning::WiFiSecurityType;

    // ===== Members that implement the NetworkProvisioningServer abstract interface

    NetworkProvisioningDelegate * _GetDelegate(void);
    void _StartPendingScan(void);
    bool _ScanInProgress(void);
    void _OnPlatformEvent(const WeaveDeviceEvent * event);

    // ===== Members that override virtual methods on NetworkProvisioningDelegate

    WEAVE_ERROR HandleScanNetworks(uint8_t networkType) override;
    WEAVE_ERROR HandleAddNetwork(PacketBuffer *networkInfoTLV) override;
    WEAVE_ERROR HandleUpdateNetwork(PacketBuffer *networkInfoTLV) override;
    WEAVE_ERROR HandleRemoveNetwork(uint32_t networkId) override;
    WEAVE_ERROR HandleGetNetworks(uint8_t flags) override;
    WEAVE_ERROR HandleEnableNetwork(uint32_t networkId) override;
    WEAVE_ERROR HandleDisableNetwork(uint32_t networkId) override;
    WEAVE_ERROR HandleTestConnectivity(uint32_t networkId) override;
    WEAVE_ERROR HandleSetRendezvousMode(uint16_t rendezvousMode) override;

    // ===== Members that override virtual methods on NetworkProvisioningServer

    bool IsPairedToAccount(void) const override;

    // ===== Members for use by the NetworkProvisioningServer implementation
    //       (both generic and platform-specific).

    enum State
    {
        kState_Idle                                     = 0x00,
        kState_ScanNetworks_Pending                     = 0x01,
        kState_ScanNetworks_InProgress                  = 0x02,
        kState_TestConnectivity_WaitWiFiConnectivity    = 0x03,
        kState_TestConnectivity_WaitThreadConnectivity  = 0x03,
    };

    uint8_t mState;
    uint8_t mScanNetworkType;
    struct
    {
        uint32_t mStatusProfileId;
        uint16_t mStatusCode;
    } mTestConnectivityResult;

    WEAVE_ERROR DoInit(void);
    int16_t GetCurrentOp(void) const;
    WEAVE_ERROR HandleAddUpdateNetwork(PacketBuffer *networkInfoTLV, bool add);
    WEAVE_ERROR HandleEnableDisableNetwork(uint32_t networkId, bool enable);
    WEAVE_ERROR ValidateWiFiStationProvision(const DeviceNetworkInfo & netInfo, uint32_t & statusProfileId, uint16_t & statusCode);
    WEAVE_ERROR ValidateThreadProvision(bool isUpdate, const DeviceNetworkInfo & netInfo, uint32_t & statusProfileId, uint16_t & statusCode);
    WEAVE_ERROR SetThreadProvisionDefaults(bool isUpdate, DeviceNetworkInfo & netInfo);
    void ContinueWiFiConnectivityTest(void);
    void ContinueThreadConnectivityTest(void);
    void HandleConnectivityTestSuccess(void);
    static void HandleConnectivityTestTimeOut(::nl::Weave::System::Layer * aLayer, void * aAppState, ::nl::Weave::System::Error aError);

private:
    ImplClass * Impl() { return static_cast<ImplClass *>(this); }
};

// Instruct the compiler to instantiate the template only when explicitly told to do so.
extern template class GenericNetworkProvisioningServerImpl<NetworkProvisioningServerImpl>;

template<class ImplClass>
inline ::nl::Weave::Profiles::NetworkProvisioning::NetworkProvisioningDelegate *
GenericNetworkProvisioningServerImpl<ImplClass>::_GetDelegate(void)
{
    return this;
}

template<class ImplClass>
inline int16_t GenericNetworkProvisioningServerImpl<ImplClass>::GetCurrentOp() const
{
    return (mCurOp != NULL) ? mCurOpType : -1;
}

template<class ImplClass>
inline bool GenericNetworkProvisioningServerImpl<ImplClass>::_ScanInProgress(void)
{
    return mState == kState_ScanNetworks_InProgress;
}

} // namespace Internal
} // namespace DeviceLayer
} // namespace Weave
} // namespace nl

#endif // NETWORK_PROVISIONING_SERVER_H
