@ stub WsAbandonCall
@ stub WsAbandonMessage
@ stub WsAbortChannel
@ stub WsAbortListener
@ stub WsAbortServiceHost
@ stub WsAbortServiceProxy
@ stub WsAcceptChannel
@ stub WsAddCustomHeader
@ stub WsAddErrorString
@ stub WsAddMappedHeader
@ stub WsAddressMessage
@ stub WsAlloc
@ stub WsAsyncExecute
@ stub WsCall
@ stub WsCheckMustUnderstandHeaders
@ stub WsCloseChannel
@ stub WsCloseListener
@ stub WsCloseServiceHost
@ stub WsCloseServiceProxy
@ stub WsCombineUrl
@ stub WsCopyError
@ stub WsCopyNode
@ stub WsCreateChannel
@ stub WsCreateChannelForListener
@ stdcall WsCreateError(ptr long ptr)
@ stub WsCreateFaultFromError
@ stdcall WsCreateHeap(long long ptr long ptr ptr)
@ stub WsCreateListener
@ stub WsCreateMessage
@ stub WsCreateMessageForChannel
@ stub WsCreateMetadata
@ stdcall WsCreateReader(ptr long ptr ptr)
@ stub WsCreateServiceEndpointFromTemplate
@ stub WsCreateServiceHost
@ stub WsCreateServiceProxy
@ stub WsCreateServiceProxyFromTemplate
@ stdcall WsCreateWriter(ptr long ptr ptr)
@ stdcall WsCreateXmlBuffer(ptr ptr long ptr ptr)
@ stub WsCreateXmlSecurityToken
@ stub WsDateTimeToFileTime
@ stub WsDecodeUrl
@ stub WsEncodeUrl
@ stub WsEndReaderCanonicalization
@ stub WsEndWriterCanonicalization
@ stub WsFileTimeToDateTime
@ stub WsFillBody
@ stdcall WsFillReader(ptr long ptr ptr)
@ stub WsFindAttribute
@ stub WsFlushBody
@ stub WsFlushWriter
@ stub WsFreeChannel
@ stdcall WsFreeError(ptr)
@ stdcall WsFreeHeap(ptr)
@ stub WsFreeListener
@ stub WsFreeMessage
@ stub WsFreeMetadata
@ stdcall WsFreeReader(ptr)
@ stub WsFreeSecurityToken
@ stub WsFreeServiceHost
@ stub WsFreeServiceProxy
@ stdcall WsFreeWriter(ptr)
@ stub WsGetChannelProperty
@ stub WsGetCustomHeader
@ stub WsGetDictionary
@ stdcall WsGetErrorProperty(ptr long ptr long)
@ stdcall WsGetErrorString(ptr long ptr)
@ stub WsGetFaultErrorDetail
@ stub WsGetFaultErrorProperty
@ stub WsGetHeader
@ stub WsGetHeaderAttributes
@ stdcall WsGetHeapProperty(ptr long ptr long ptr)
@ stub WsGetListenerProperty
@ stub WsGetMappedHeader
@ stub WsGetMessageProperty
@ stub WsGetMetadataEndpoints
@ stub WsGetMetadataProperty
@ stub WsGetMissingMetadataDocumentAddress
@ stub WsGetNamespaceFromPrefix
@ stub WsGetOperationContextProperty
@ stub WsGetPolicyAlternativeCount
@ stub WsGetPolicyProperty
@ stub WsGetPrefixFromNamespace
@ stdcall WsGetReaderNode(ptr ptr ptr)
@ stub WsGetReaderPosition
@ stdcall WsGetReaderProperty(ptr long ptr long ptr)
@ stub WsGetSecurityContextProperty
@ stub WsGetSecurityTokenProperty
@ stub WsGetServiceHostProperty
@ stub WsGetServiceProxyProperty
@ stub WsGetWriterPosition
@ stdcall WsGetWriterProperty(ptr long ptr long ptr)
@ stdcall WsGetXmlAttribute(ptr ptr ptr ptr ptr ptr)
@ stub WsInitializeMessage
@ stub WsMarkHeaderAsUnderstood
@ stub WsMatchPolicyAlternative
@ stub WsMoveReader
@ stub WsMoveWriter
@ stub WsOpenChannel
@ stub WsOpenListener
@ stub WsOpenServiceHost
@ stub WsOpenServiceProxy
@ stub WsPullBytes
@ stub WsPushBytes
@ stub WsReadArray
@ stub WsReadAttribute
@ stub WsReadBody
@ stub WsReadBytes
@ stub WsReadChars
@ stub WsReadCharsUtf8
@ stub WsReadElement
@ stub WsReadEndAttribute
@ stdcall WsReadEndElement(ptr ptr)
@ stub WsReadEndpointAddressExtension
@ stub WsReadEnvelopeEnd
@ stub WsReadEnvelopeStart
@ stub WsReadMessageEnd
@ stub WsReadMessageStart
@ stub WsReadMetadata
@ stdcall WsReadNode(ptr ptr)
@ stub WsReadQualifiedName
@ stub WsReadStartAttribute
@ stdcall WsReadStartElement(ptr ptr)
@ stdcall WsReadToStartElement(ptr ptr ptr ptr ptr)
@ stdcall WsReadType(ptr long long ptr long ptr ptr long ptr)
@ stub WsReadValue
@ stub WsReadXmlBuffer
@ stub WsReadXmlBufferFromBytes
@ stub WsReceiveMessage
@ stub WsRegisterOperationForCancel
@ stub WsRemoveCustomHeader
@ stub WsRemoveHeader
@ stub WsRemoveMappedHeader
@ stub WsRemoveNode
@ stub WsRequestReply
@ stub WsRequestSecurityToken
@ stub WsResetChannel
@ stub WsResetError
@ stub WsResetHeap
@ stub WsResetListener
@ stub WsResetMessage
@ stub WsResetMetadata
@ stub WsResetServiceHost
@ stub WsResetServiceProxy
@ stub WsRevokeSecurityContext
@ stub WsSendFaultMessageForError
@ stub WsSendMessage
@ stub WsSendReplyMessage
@ stub WsSetChannelProperty
@ stdcall WsSetErrorProperty(ptr long ptr long)
@ stub WsSetFaultErrorDetail
@ stub WsSetFaultErrorProperty
@ stub WsSetHeader
@ stdcall WsSetInput(ptr ptr ptr ptr long ptr)
@ stdcall WsSetInputToBuffer(ptr ptr ptr long ptr)
@ stub WsSetListenerProperty
@ stub WsSetMessageProperty
@ stdcall WsSetOutput(ptr ptr ptr ptr long ptr)
@ stdcall WsSetOutputToBuffer(ptr ptr ptr long ptr)
@ stub WsSetReaderPosition
@ stub WsSetWriterPosition
@ stub WsShutdownSessionChannel
@ stub WsSkipNode
@ stub WsStartReaderCanonicalization
@ stub WsStartWriterCanonicalization
@ stub WsTrimXmlWhitespace
@ stub WsVerifyXmlNCName
@ stub WsWriteArray
@ stub WsWriteAttribute
@ stub WsWriteBody
@ stub WsWriteBytes
@ stub WsWriteChars
@ stub WsWriteCharsUtf8
@ stub WsWriteElement
@ stub WsWriteEndAttribute
@ stub WsWriteEndCData
@ stub WsWriteEndElement
@ stub WsWriteEndStartElement
@ stub WsWriteEnvelopeEnd
@ stub WsWriteEnvelopeStart
@ stub WsWriteMessageEnd
@ stub WsWriteMessageStart
@ stub WsWriteNode
@ stub WsWriteQualifiedName
@ stub WsWriteStartAttribute
@ stub WsWriteStartCData
@ stub WsWriteStartElement
@ stub WsWriteText
@ stub WsWriteType
@ stub WsWriteValue
@ stub WsWriteXmlBuffer
@ stub WsWriteXmlBufferToBytes
@ stub WsWriteXmlnsAttribute
@ stub WsXmlStringEquals
