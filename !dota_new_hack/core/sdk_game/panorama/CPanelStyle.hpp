class CStyleProperty;

class CStyleSymbol;
class CUILength;

class CStyleFactoryWrapper {
public:
	virtual bool BRegisteredProperty( CStyleSymbol symName ) = 0;
	virtual CStyleSymbol BRegisteredAlias( CStyleSymbol symName ) = 0;
	virtual CStyleSymbol GetPropertyNameForAlias( unsigned __int8* a2, unsigned __int8* a3 ) = 0;
	virtual CStyleProperty* CreateStyleProperty( CStyleSymbol symName ) = 0;
	virtual void FreeStyleProperty( CStyleProperty* pProperty ) = 0;
	// virtual const CUtlVector< CUtlString >& GetSortedPropertyAndAliasNames( ) = 0;
};

class CPanelStyle {
public:
	virtual void Clear( bool idk ) = 0; // 0x0
	virtual void* GetUIScaleFactor( void ) = 0; // 0x8
	virtual bool BHasAnyStyleDataForProperty( CStyleSymbol hSymbolProperty ) = 0;  // 0x10
	virtual void GetPosition( CUILength& x, CUILength& y, CUILength& z, bool bIncludeUIScaleFactor = true ) = 0;// 0x18
	virtual void GetInterpolatedPosition( CUILength& x, CUILength& y, CUILength& z, bool bFinal, bool bIncludeUIScaleFactor = true ) = 0; // 0x20
	virtual void SetPosition( CUILength x, CUILength y, CUILength z, bool bPreScaledByUIScaleFactor = false ) = 0; // 0x28
	virtual void SetPositionWithoutTransition( CUILength x, CUILength y, CUILength z, bool bPreScaledByUIScaleFactor = false ) = 0; // 0x30
	virtual void func7( ) = 0; // 0x38
	virtual void func8( ) = 0; // 0x40
	virtual void func9( ) = 0; // 0x48
	virtual void func10( ) = 0; // 0x50
	virtual void func11( ) = 0; // 0x58
	virtual void func12( ) = 0; // 0x60
	virtual void func13( ) = 0; // 0x68
	virtual void func14( ) = 0; // 0x70
	virtual void func15( ) = 0; // 0x78
	virtual void func16( ) = 0; // 0x80
	virtual void func17( ) = 0; // 0x88
	virtual void func18( ) = 0; // 0x90
	virtual void func19( ) = 0; // 0x98
	virtual void func20( ) = 0; // 0xa0
	virtual void func21( ) = 0; // 0xa8
	virtual void SetOpacity( float a1 ) = 0; // 0xb0
	virtual void func23( ) = 0; // 0xb8
	virtual void func24( ) = 0; // 0xc0
	virtual void func25( ) = 0; // 0xc8
	virtual void func26( ) = 0; // 0xd0
	virtual void func27( ) = 0; // 0xd8
	virtual void func28( ) = 0; // 0xe0
	virtual void func29( ) = 0; // 0xe8
	virtual void func30( ) = 0; // 0xf0
	virtual void func31( ) = 0; // 0xf8
	virtual void func32( ) = 0; // 0x100
	virtual void func33( ) = 0; // 0x108
	virtual void func34( ) = 0; // 0x110
	virtual void func35( ) = 0; // 0x118
	virtual void func36( ) = 0; // 0x120
	virtual void func37( ) = 0; // 0x128
	virtual void func38( ) = 0; // 0x130
	virtual void func39( ) = 0; // 0x138
	virtual void func40( ) = 0; // 0x140
	virtual void func41( ) = 0; // 0x148
	virtual void func42( ) = 0; // 0x150
	virtual void func43( ) = 0; // 0x158
	virtual void func44( ) = 0; // 0x160
	virtual void func45( ) = 0; // 0x168
	virtual void func46( ) = 0; // 0x170
	virtual void func47( ) = 0; // 0x178
	virtual void func48( ) = 0; // 0x180
	virtual void func49( ) = 0; // 0x188
	virtual void SetBackgroundColor( const char* color ) = 0; // 0x190
	virtual void func51( ) = 0; // 0x198
	virtual void func52( ) = 0; // 0x1a0
	virtual void SetForegroundColor( const char* color ) = 0; // 0x1a8
	virtual void func54( ) = 0; // 0x1b0
	virtual void func55( ) = 0; // 0x1b8
	virtual void func56( ) = 0; // 0x1c0
	virtual void func57( ) = 0; // 0x1c8
	virtual void func58( ) = 0; // 0x1d0
	virtual void func59( ) = 0; // 0x1d8
	virtual void func60( ) = 0; // 0x1e0
	virtual void func61( ) = 0; // 0x1e8
	virtual void func62( ) = 0; // 0x1f0
	virtual void func63( ) = 0; // 0x1f8
	virtual void func64( ) = 0; // 0x200
	virtual void func65( ) = 0; // 0x208
	virtual void func66( ) = 0; // 0x210
	virtual void func67( ) = 0; // 0x218
	virtual void func68( ) = 0; // 0x220
	virtual void func69( ) = 0; // 0x228
	virtual void func70( ) = 0; // 0x230
	virtual void func71( ) = 0; // 0x238
	virtual void func72( ) = 0; // 0x240
	virtual void func73( ) = 0; // 0x248
	virtual void func74( ) = 0; // 0x250
	virtual void func75( ) = 0; // 0x258
	virtual void func76( ) = 0; // 0x260
	virtual void func77( ) = 0; // 0x268
	virtual void func78( ) = 0; // 0x270
	virtual void func79( ) = 0; // 0x278
	virtual void func80( ) = 0; // 0x280
	virtual void func81( ) = 0; // 0x288
	virtual void func82( ) = 0; // 0x290
	virtual void func83( ) = 0; // 0x298
	virtual void func84( ) = 0; // 0x2a0
	virtual void func85( ) = 0; // 0x2a8
	virtual void func86( ) = 0; // 0x2b0
	virtual void func87( ) = 0; // 0x2b8
	virtual bool GetVisiblity( bool* out ) = 0; // 0x2c0
	virtual void set_visibility( bool vis ) = 0; // 0x2c8
	virtual void func90( ) = 0; // 0x2d0
	virtual void func91( ) = 0; // 0x2d8
	virtual void func92( ) = 0; // 0x2e0
	virtual void func93( ) = 0; // 0x2e8
	virtual void func94( ) = 0; // 0x2f0
	virtual void func95( ) = 0; // 0x2f8
	virtual void func96( ) = 0; // 0x300
	virtual void func97( ) = 0; // 0x308
	virtual void func98( ) = 0; // 0x310
	virtual void func99( ) = 0; // 0x318
	virtual void func100( ) = 0; // 0x320
	virtual void func101( ) = 0; // 0x328
	virtual void func102( ) = 0; // 0x330
	virtual void func103( ) = 0; // 0x338
	virtual void func104( ) = 0; // 0x340
	virtual void func105( ) = 0; // 0x348
	virtual void func106( ) = 0; // 0x350
	virtual void func107( ) = 0; // 0x358
	virtual void func108( ) = 0; // 0x360
	virtual void func109( ) = 0; // 0x368
	virtual void func110( ) = 0; // 0x370
	virtual void func111( ) = 0; // 0x378
	virtual void func112( ) = 0; // 0x380
	virtual void func113( ) = 0; // 0x388
	virtual void func114( ) = 0; // 0x390
	virtual void func115( ) = 0; // 0x398
	virtual void func116( ) = 0; // 0x3a0
	virtual void func117( ) = 0; // 0x3a8
	virtual void func118( ) = 0; // 0x3b0
	virtual void func119( ) = 0; // 0x3b8
	virtual void func120( ) = 0; // 0x3c0
	virtual void func121( ) = 0; // 0x3c8
	virtual void func122( ) = 0; // 0x3d0
	virtual void func123( ) = 0; // 0x3d8
	virtual void func124( ) = 0; // 0x3e0
	virtual void func125( ) = 0; // 0x3e8
	virtual void func126( ) = 0; // 0x3f0
	virtual void func127( ) = 0; // 0x3f8
	virtual void func128( ) = 0; // 0x400
	virtual void func129( ) = 0; // 0x408
	virtual void func130( ) = 0; // 0x410
	virtual void func131( ) = 0; // 0x418
	virtual void func132( ) = 0; // 0x420
	virtual void func133( ) = 0; // 0x428
	virtual void func134( ) = 0; // 0x430
	virtual void func135( ) = 0; // 0x438
	virtual void func136( ) = 0; // 0x440
	virtual void func137( ) = 0; // 0x448
	virtual void func138( ) = 0; // 0x450
	virtual void func139( ) = 0; // 0x458
	virtual void func140( ) = 0; // 0x460
	virtual void func141( ) = 0; // 0x468
	virtual void func142( ) = 0; // 0x470
	virtual void func143( ) = 0; // 0x478
	virtual void func144( ) = 0; // 0x480
	virtual void func145( ) = 0; // 0x488
	virtual void func146( ) = 0; // 0x490
};