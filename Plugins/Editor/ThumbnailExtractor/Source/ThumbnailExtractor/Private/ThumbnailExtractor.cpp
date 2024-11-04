// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThumbnailExtractor.h"

#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "IContentBrowserSingleton.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "Playset.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Subsystems/EditorAssetSubsystem.h"

#define LOCTEXT_NAMESPACE "FThumbnailExtractorModule"

namespace AssetMenuExtension_ThumbnailExtractor
{
	static void AddAssetMenuExtension()
	{
		TArray<UToolMenu*> Menus = UE::ContentBrowser::ExtendToolMenu_AssetContextMenus({
			UStaticMesh::StaticClass(), UPlayset::StaticClass()
		});
		for (UToolMenu* Menu : Menus)
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");

			auto ToolMenuAction = FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext& Context)
			{
				UContentBrowserAssetContextMenuContext* CBContext = Context.FindContext<
					UContentBrowserAssetContextMenuContext>();
				if (CBContext == nullptr)
				{
					return;
				}

				for (const FAssetData& AssetData : CBContext->SelectedAssets)
				{
					UE_LOG(LogAssetData, Display, TEXT("Selected asset %s"), *AssetData.AssetName.ToString());
					
					UTexture2D* CreatedTexture = nullptr;

					FString PackageFilename;
					const FName ObjectFullName = FName(*AssetData.GetFullName());
					TSet<FName> ObjectFullNames;
					ObjectFullNames.Add(ObjectFullName);
					if (FPackageName::DoesPackageExist(AssetData.PackageName.ToString(), &PackageFilename))
					{
						FThumbnailMap ThumbnailMap;
						ThumbnailTools::LoadThumbnailsFromPackage(PackageFilename, ObjectFullNames,
						                                          ThumbnailMap);

						FObjectThumbnail* objTN = ThumbnailMap.Find(ObjectFullName);
						IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<
							IImageWrapperModule>(TEXT("ImageWrapper"));
						TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(
							EImageFormat::PNG);

						ImageWrapper->SetRaw(objTN->GetUncompressedImageData().GetData(),
						                     objTN->GetUncompressedImageData().Num(), objTN->GetImageWidth(),
						                     objTN->GetImageHeight(), ERGBFormat::BGRA, 8);
						const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();

						CreatedTexture = FImageUtils::ImportBufferAsTexture2D(CompressedByteArray);

						UE_LOG(LogAssetData, Display, TEXT("		Created texture %s"), *CreatedTexture->GetName());

						// Save the texture to the asset's directory
						FString AssetPath = FPackageName::GetLongPackagePath(AssetData.PackageName.ToString());
						FString AssetName = FPackageName::GetShortName(AssetData.PackageName.ToString());
						FString AssetDirectory = FPaths::Combine(AssetPath, AssetName);
						FString TextureName = FString::Printf(TEXT("%s_Thumbnail"), *AssetName);
						FString TexturePath = FPaths::Combine(AssetDirectory, TextureName);

						FSaveAssetDialogConfig SaveConfig;
						SaveConfig.DialogTitleOverride = LOCTEXT("SaveThumbnailDialogTitle", "Save Thumbnail");
						SaveConfig.DefaultPath = TexturePath;
						SaveConfig.DefaultAssetName = TextureName;
						SaveConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;

						const FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
						const FString SaveObjPath = CBModule.Get().CreateModalSaveAssetDialog(SaveConfig);
						if (!SaveObjPath.IsEmpty())
						{
							const FString SavePackageName = FPackageName::ObjectPathToPackageName(SaveObjPath);
							const FString SavePackagePath = FPaths::GetPath(SavePackageName);
							const FString SaveAssetName = FPaths::GetBaseFilename(SavePackageName);

							UPackage* Package = CreatePackage(*SavePackageName);
							if (ensure(Package))
							{
								// Create and init a new texture 2D
								Package->FullyLoad();

								UTexture2D* NewTex = NewObject<UTexture2D>(Package, UTexture2D::StaticClass(), *SaveAssetName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
								{
									NewTex->AddToRoot();
									NewTex->SetPlatformData(new FTexturePlatformData());
									NewTex->GetPlatformData()->SizeX = CreatedTexture->GetSizeX();
									NewTex->GetPlatformData()->SizeY = CreatedTexture->GetSizeY();
									NewTex->GetPlatformData()->PixelFormat = PF_B8G8R8A8;
									NewTex->GetPlatformData()->SetNumSlices(1);

									FTexture2DMipMap* CopyMip = &CreatedTexture->GetPlatformData()->Mips[0];
									FByteBulkData* RawImageData = &CopyMip->BulkData;
									FColor* FormatedImageData = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

									auto GatherPixelColor = [FormatedImageData, CreatedTexture] (int32 x, int32 y)
									{
										return FormatedImageData[y * CreatedTexture->GetSizeX() + x];
									};

									uint8* Pixels = new uint8[CreatedTexture->GetSizeX() * CreatedTexture->GetSizeY() * 4];
									for (int32 y = 0; y < CreatedTexture->GetSizeY(); y++)
									{
										for (int32 x = 0; x < CreatedTexture->GetSizeX(); x++)
										{
											int32 curPixelIdx = ((y * CreatedTexture->GetSizeX() + x));
											Pixels[4 * curPixelIdx] = GatherPixelColor(x, y).B;
											Pixels[4 * curPixelIdx + 1] = GatherPixelColor(x, y).G;
											Pixels[4 * curPixelIdx + 2] = GatherPixelColor(x, y).R;
											Pixels[4 * curPixelIdx + 3] = GatherPixelColor(x, y).A;
										}
									}
									CopyMip->BulkData.Unlock();
									
									FTexture2DMipMap* Mip = new FTexture2DMipMap();
									Mip->SizeX = CreatedTexture->GetSizeX();
									Mip->SizeY = CreatedTexture->GetSizeY();
									Mip->BulkData.Lock(LOCK_READ_WRITE);
									uint8* TexData = (uint8*) Mip->BulkData.Realloc(CreatedTexture->GetSizeX() * CreatedTexture->GetSizeY() * 4);
									FMemory::Memcpy(TexData, Pixels, sizeof(uint8) * CreatedTexture->GetSizeX() * CreatedTexture->GetSizeY() * 4);
									Mip->BulkData.Unlock();

									NewTex->Source.Init(CreatedTexture->GetSizeX(), CreatedTexture->GetSizeY(), 1, 1, TSF_BGRA8, Pixels);
									NewTex->UpdateResource();
									Package->MarkPackageDirty();
									FAssetRegistryModule::AssetCreated(NewTex);
									NewTex->PostEditChange();
									GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(NewTex);

									UPackage::SavePackage(Package, NewTex, EObjectFlags::RF_Public | RF_Standalone, *SavePackagePath, GError);
									delete[] Pixels;
								}
							}
						}
					}
				}
			});

			FToolUIActionChoice ExtractThumbnailAction(ToolMenuAction);
			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				"ExtractThumbnail",
				LOCTEXT("ExtractThumbnail", "Extract Thumbnail"),
				LOCTEXT("ExtractThumbnailTooltip",
				        "Extract the thumbnail from the selected asset and save it to the asset's directory."),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.StaticMesh"),
				ExtractThumbnailAction
			));
		}
	}
}

void FThumbnailExtractorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([]()
	{
		AssetMenuExtension_ThumbnailExtractor::AddAssetMenuExtension();
	});
}

void FThumbnailExtractorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FThumbnailExtractorModule, ThumbnailExtractor)
