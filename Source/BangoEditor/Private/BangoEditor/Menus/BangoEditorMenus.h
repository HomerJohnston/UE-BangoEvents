#pragma once

class UBangoActorIDComponent;

struct FBangoEditorMenus
{
	FBangoEditorMenus();
	
	static void BindCommands();
	
	static void BuildMenus();
	
	static void SetEditActorID();
	
	static void SetEditActorID(AActor* Actor, bool bBlocking = false);
	
	static bool Can_SetEditActorID();
	
	static TSharedPtr<SWindow> ActiveWindow;
	
	static TSharedRef<SWidget> GetIDEditingWidget(TSharedPtr<SEditableTextBox> InputBox);
	
	static TSharedRef<SWidget> GetTitleWidget();
	
	static UBangoActorIDComponent* CreateIDComponent(TWeakObjectPtr<AActor> WeakActor);
	
	static void DestroyIDComponent(TWeakObjectPtr<AActor> WeakActor, TWeakObjectPtr<UBangoActorIDComponent> WeakExistingIDComponent);
	
	static void OnTextCommitted_IDField(const FText& InText, ETextCommit::Type InCommitType, TWeakObjectPtr<AActor> WeakActor, TWeakObjectPtr<UBangoActorIDComponent> WeakIDComponent);
	
	static TSharedPtr<SEditableTextBox> CreateInputBox(TWeakObjectPtr<AActor> WeakActor, TWeakObjectPtr<UBangoActorIDComponent> WeakIDComponent);
};
