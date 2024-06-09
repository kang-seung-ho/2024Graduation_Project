CREATE TABLE [dbo].[UserDataTable]
(
	[Id] CHAR(16) NOT NULL PRIMARY KEY, 
	[Password] CHAR(32) NOT NULL,
	[LastSignInTime] DATETIME NULL,
)
